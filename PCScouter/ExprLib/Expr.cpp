//
// Copyright(C) 2020 by Jack (Butch) Griffin
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see < https://www.gnu.org/licenses/>.
//
//
//
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
//

#include "Expr.h"
#include "ExprNode.h"
#include "ExprOperator.h"
#include "ExprConst.h"
#include "ExprVariable.h"
#include "ExprFunction.h"
#include <QRegularExpression>
#include <QDebug>
#include <stack>

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

		bool Expr::parse(ExprContext& ctxt, const QString& txt, QString& err)
		{
			int index = 0;
			top_ = parseSubExpr(ctxt, txt, index, err);
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
			std::stack<std::shared_ptr<ExprNode>> operands;
			std::stack<std::shared_ptr<ExprOperator>> operators;
			std::shared_ptr<ExprNode> operand1, operand2;
			std::shared_ptr<ExprOperator> op1, op2;

			if (index == txt.length())
			{
				err = "unexpected end of string";
				return nullptr;
			}

			operand1 = parseNode(context, txt, index, err);
			if (operand1 == nullptr)
				return nullptr;

			operands.push(operand1);

			op1 = parseOper(context, txt, index, err);
			if (op1 == nullptr)
			{
				err.clear();
				return operand1;
			}
			operators.push(op1);

			while (true)
			{
				operand2 = parseNode(context, txt, index, err);
				if (operand2 == nullptr)
					return nullptr;

				operands.push(operand2);

				op2 = parseOper(context, txt, index, err);
				if (op2 == nullptr)
				{
					while (operators.size() > 0)
						reduce(operands, operators);

					operand1 = operands.top();
					err.clear();
					return operand1;
				}

				while (operators.size() > 0)
				{
					op1 = operators.top();
					if (op1->prec() > op2->prec())
						break;

					reduce(operands, operators);
				}

				operators.push(op2);
			}

			return nullptr;
		}

		void Expr::reduce(std::stack<std::shared_ptr<ExprNode>> &operands, std::stack<std::shared_ptr<ExprOperator>>& operators)
		{
			auto op1 = operators.top();
			operators.pop();

			op1->addNode(operands.top());
			operands.pop();
			op1->addNode(operands.top());
			operands.pop();
			operands.push(op1);
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
				QRegularExpression exp("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?");
				QRegularExpressionMatch m = exp.match(txt.mid(index));
				if (m.hasMatch())
				{
					index += m.captured(0).length();
					QString str = m.captured(0);
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

				skipSpaces(txt, index);

				if (index < txt.length() && txt[index] == '(')
				{
					std::vector<std::shared_ptr<ExprNode>> arguments;
					index++;

					//
					// This is a function, parse the arguments
					//
					while (true)
					{
						auto arg = parseSubExpr(context, txt, index, err);
						if (arg == nullptr)
							return nullptr;

						arguments.push_back(arg);

						skipSpaces(txt, index);
						if (index == txt.length())
						{
							err = "function '" + id + "' missing trailing ')'";
							return nullptr;
						}

						QString imd = txt.mid(index);
						if (txt[index] == ')')
						{
							break;
						}
						else if (txt[index] != ',')
						{
							err = "function '" + id + "' expected ',' or ')', got '" + QString(txt[index]) + "'";
							return nullptr;
						}

						index++;
						skipSpaces(txt, index);
					}

					index++;
					ret = std::make_shared<ExprFunction>(this, id, arguments);

				}
				else
				{
					if (id == "true" || id == "TRUE")
					{
						ret = std::make_shared<ExprConst>(this, QVariant(true));
					}
					else if (id == "false" || id == "FALSE")
					{
						ret = std::make_shared<ExprConst>(this, QVariant(false));
					}
					else if (context.isValidVariable(id))
					{
						ret = std::make_shared<ExprVariable>(this, id);
					}
					else
					{
						err = "Identifier '" + id + "' is invalid";
					}
				}
			}

			return ret;
		}

		std::shared_ptr<ExprOperator> Expr::parseOper(ExprContext& context, const QString& txt, int& index, QString& err)
		{
			std::shared_ptr<ExprOperator> result;

			(void)context;

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
			else if (txt.at(index) == '<')
			{
				result = std::make_shared<ExprOperator>(this, ExprOperator::OpType::LessThan);
				index++;
			}
			else if (txt.at(index) == '>')
			{
				result = std::make_shared<ExprOperator>(this, ExprOperator::OpType::GreaterThan);
				index++;
			}
			else if (index < txt.length() - 1 && txt.mid(index, 2) == "==")
			{
				result = std::make_shared<ExprOperator>(this, ExprOperator::OpType::Equal);
				index += 2;
			}
			else if (index < txt.length() - 1 && txt.mid(index, 2) == "!=")
			{
				result = std::make_shared<ExprOperator>(this, ExprOperator::OpType::NotEqual);
				index += 2;
			}
			else if (index < txt.length() - 1 && txt.mid(index, 2) == ">=")
			{
				result = std::make_shared<ExprOperator>(this, ExprOperator::OpType::GreaterEqual);
				index += 2;
			}
			else if (index < txt.length() - 1 && txt.mid(index, 2) == "<")
			{
				result = std::make_shared<ExprOperator>(this, ExprOperator::OpType::LessThanEqual);
				index += 2;
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