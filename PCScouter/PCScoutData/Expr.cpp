#include "Expr.h"
#include "ExprNode.h"
#include "ExprOperator.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
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

			bool Expr::parse(const QString& text, QString& err)
			{
				int index = 0;
				top_ = parseSubExpr(text, index, err);
				if (top_ == nullptr)
					return false;

				return false;
			}

			void Expr::skipSpaces(const QString& txt, int& index)
			{
				while (index < txt.length() && txt[index].isSpace())
					index++;
			}

			std::shared_ptr<ExprNode> Expr::parseSubExpr(const QString& txt, int& index, QString& err)
			{
				std::shared_ptr<ExprNode> operand1, operand2;
				std::shared_ptr<ExprOperator> oper1, oper2;
				std::shared_ptr<ExprNode> top;

				if (index == txt.length())
				{
					err = "unexpected and of string";
					return nullptr;
				}

				if (txt[index] == '(')
				{
					index++;
					auto result = parseSubExpr(txt, index, err);
					if (result != nullptr)
					{
						skipSpaces(txt, index);

						if (index == txt.length())
						{
							err = "missing trailing ')' character";
							result = nullptr;
						}
						else if (txt[index] != ')')
						{
							err = "expected ')' character, got '" + QString(txt[index]) + "'";
							result = nullptr;
						}

						index++;
					}
					return result;
				}

				operand1 = parseNode(txt, index, err);
				if (operand1 == nullptr)
					return nullptr;

				oper1 = parseOper(txt, index, err);
				if (oper1 == nullptr)
					return operand1;

				top = oper1;
				oper1->addNode(operand1);

				while (true)
				{
					operand2 = parseNode(txt, index, err);
					if (operand2 == nullptr)
						return nullptr;

					oper2 = parseOper(txt, index, err);
					if (oper2 == nullptr)
					{
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

			std::shared_ptr<ExprNode> Expr::parseNode(const QString& txt, int &index, QString& err)
			{
				skipSpaces(txt, index);
				if (index == txt.length())
				{
					err = "end of text searching for operand";
					return nullptr;
				}

				QChar ch = txt.at(index);
				if (ch == '-' || ch == '+' || ch.isDigit())
				{

				}
				else
				{

				}
			}

			std::shared_ptr<ExprOperator> Expr::parseOper(const QString& txt, int &index, QString& err)
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
				}
				else if (txt.at(index) == '+')
				{
					result = std::make_shared<ExprOperator>(this, ExprOperator::OpType::Minus);
				}
				else if (txt.at(index) == '*')
				{
					result = std::make_shared<ExprOperator>(this, ExprOperator::OpType::Times);
				}
				else if (txt.at(index) == '/')
				{
					result = std::make_shared<ExprOperator>(this, ExprOperator::OpType::Divide);
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

			QVariant Expr::eval(ExprEvalContext &ctxt) const
			{
				return top_->eval(ctxt);
			}
		}
	}
}
