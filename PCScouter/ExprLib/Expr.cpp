#include "Expr.h"
#include "ExprNode.h"
#include "ExprOperator.h"
#include "ExprConst.h"
#include "ExprVariable.h"
#include <QRegExp>

namespace xero
{
	namespace expr
	{
		Expr::Expr()
		{
		}

		Expr::~Expr()
		{
		}

		QString Expr::toString() const
		{
			return top_->toString();
		}

		bool Expr::parse(ExprContext &ctxt, const QString& text, QString& err)
		{
			int index = 0;
			top_ = parseSubExpr(ctxt, text, index, err);
			if (top_ == nullptr)
				return false;

			return true;
		}

		void Expr::skipSpaces(const QString& txt, int& index)
		{
			while (index < txt.length() && txt[index].isSpace())
				index++;
		}

		std::shared_ptr<ExprNode> Expr::parseSubExpr(ExprContext& context, const QString& txt, int& index, QString& err)
		{
			std::shared_ptr<ExprNode> operand1, operand2;
			std::shared_ptr<ExprOperator> oper1, oper2;
			std::shared_ptr<ExprNode> top;

			if (index == txt.length())
			{
				err = "unexpected and of string";
				return nullptr;
			}

			operand1 = parseNode(context, txt, index, err);
			if (operand1 == nullptr)
				return nullptr;

			oper1 = parseOper(context, txt, index, err);
			if (oper1 == nullptr)
			{
				err.clear();
				return operand1;
			}

			top = oper1;
			oper1->addNode(operand1);

			while (true)
			{
				operand2 = parseNode(context, txt, index, err);
				if (operand2 == nullptr)
					return nullptr;

				oper2 = parseOper(context, txt, index, err);
				if (oper2 == nullptr)
				{
					err.clear();
					oper1->addNode(operand2);
					return oper1;
				}

				if (oper1->prec() > oper2->prec())
				{
					oper1->addNode(operand2);
					oper2->addNode(oper1);
					oper1 = oper2;
				}
				else
				{
					oper1->addNode(operand2);
					oper2->addNode(top);
					oper1 = oper2;
					top = oper1;
				}
			}

			return nullptr;
		}

		std::shared_ptr<ExprNode> Expr::parseNode(ExprContext& context, const QString& txt, int& index, QString& err)
		{
			std::shared_ptr<ExprNode> ret;

			skipSpaces(txt, index);
			if (index == txt.length())
			{
				err = "end of text searching for operand";
				return nullptr;
			}

			QChar ch = txt.at(index);
			if (ch == '(')
			{
				index++;

				ret = parseSubExpr(context, txt, index, err);
				if (ret == nullptr)
					return nullptr;

				skipSpaces(txt, index);

				if (index == txt.length())
				{
					err = "end of string while expected ')' character";
					return nullptr;
				}

				if (txt.at(index) != ')')
				{
					err = "got '" + txt.mid(index, 1) + "' while expectedin ')'";
					return nullptr;
				}

				index++;
			}
			else if (ch == '+' || ch == '-' || ch.isDigit())
			{
				QRegExp exp("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?");
				exp.exactMatch(txt.mid(index));
				int len = exp.matchedLength();
				if (len > 0)
				{
					index += len;
					QString str = exp.cap(0);
					bool ok;

					int v = str.toInt(&ok);
					if (ok)
					{
						ret = std::make_shared<ExprConst>(this, QVariant(v));
					}
					else
					{
						double r = str.toDouble(&ok);
						if (ok)
						{
							ret = std::make_shared<ExprConst>(this, QVariant(r));
						}
						else
						{
							err = "Invalid constant value '" + str + "'";
						}
					}

				}
				else
				{
					err = "invalid integer or floating point constant";
				}
			}
			else if (ch.isLetter() || ch == '_')
			{
				QString id;
				id += ch;
				index++;

				while (index < txt.length())
				{
					ch = txt.at(index);
					if (!ch.isLetterOrNumber() && ch != '_')
						break;

					id += ch;
					index++;
				}

				if (context.isValidVariable(id))
				{
					ret = std::make_shared<ExprVariable>(this, id);
				}
				else
				{
					err = "Identifier '" + id + "' is invalid";
				}
			}

			return ret;
		}

		std::shared_ptr<ExprOperator> Expr::parseOper(ExprContext& context, const QString& txt, int& index, QString& err)
		{
			std::shared_ptr<ExprOperator> result;

			skipSpaces(txt, index);
			if (index == txt.length())
			{
				err = "end of text searching for operator";
				return nullptr;
			}

			if (txt.at(index) == '+')
			{
				result = std::make_shared<ExprOperator>(this, ExprOperator::OpType::Plus);
				index++;
			}
			else if (txt.at(index) == '-')
			{
				result = std::make_shared<ExprOperator>(this, ExprOperator::OpType::Minus);
				index++;
			}
			else if (txt.at(index) == '*')
			{
				result = std::make_shared<ExprOperator>(this, ExprOperator::OpType::Times);
				index++;
			}
			else if (txt.at(index) == '/')
			{
				result = std::make_shared<ExprOperator>(this, ExprOperator::OpType::Divide);
				index++;
			}
			else
			{
				err = "expected operator, got '" + QString(txt.at(index)) + "'";
			}

			return result;
		}

		QStringList Expr::allVariables() const
		{
			QStringList list;
			top_->allVariables(list);

			return list;
		}

		QVariant Expr::eval(ExprContext& ctxt) const
		{
			return top_->eval(ctxt);
		}
	}
}