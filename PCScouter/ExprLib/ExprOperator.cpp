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

#include "ExprOperator.h"
#include <cmath>

namespace xero
{
	namespace expr
	{
		ExprOperator::ExprOperator(Expr* e, OpType op) : ExprNode(e)
		{
			op_ = op;
		}

		ExprOperator::~ExprOperator()
		{
		}

		int ExprOperator::prec()
		{
			int result = 0;

			switch (op_)
			{
			case OpType::Plus:
				result = 4;
				break;
			case OpType::Minus:
				result = 4;
				break;
			case OpType::Times:
				result = 3;
				break;
			case OpType::Divide:
				result = 3;
				break;
			case OpType::LessThan:
			case OpType::GreaterThan:
			case OpType::GreaterEqual:
			case OpType::LessThanEqual:
				result = 6;
				break;
			case OpType::Equal:
			case OpType::NotEqual:
				result = 7;
				break;
			case OpType::LogicalOr:
				result = 11;
				break;
			case OpType::LogicalAnd:
				result = 12;
				break;
			}

			return result;
		}

		QVariant ExprOperator::eval(ExprContext& ctxt)
		{
			assert(nodes_.size() != 0);

			QVariant result = nodes_[0]->eval(ctxt);
			for (size_t i = 1; i < nodes_.size(); i++)
			{
				QVariant r = nodes_[i]->eval(ctxt);
				switch (op_)
				{
				case OpType::Plus:
					result = plus(result, r);
					break;
				case OpType::Minus:
					result = minus(result, r);
					break;
				case OpType::Times:
					result = times(result, r);
					break;
				case OpType::Divide:
					result = divide(result, r);
					break;
				case OpType::LessThan:
					result = lessthan(result, r);
					break;
				case OpType::GreaterThan:
					result = greaterthan(result, r);
					break;
				case OpType::Equal:
					result = equal(result, r);
					break;
				case OpType::NotEqual:
					result = notequal(result, r);
					break;
				case OpType::GreaterEqual:
					result = greaterequal(result, r);
					break;
				case OpType::LessThanEqual:
					result = lessthanequal(result, r);
					break;
				case OpType::LogicalAnd:
					result = logicaland(result, r);
					break;
				case OpType::LogicalOr:
					result = logicalor(result, r);
					break;
				}
			}

			return result;
		}

		QVariant ExprOperator::lessthan(const QVariant& v1, const QVariant& v2)
		{
			QVariant result;

			if (v1.type() == QVariant::Type::String && v2.type() == QVariant::Type::String)
				result = v1.toString() < v2.toString();
			else if (v1.type() == QVariant::Int && v2.type() == QVariant::Type::Int)
				result = v1.toInt() < v2.toInt();
			else if (v1.canConvert(QVariant::Type::Double) && v1.canConvert(QVariant::Type::Double))
				result = v1.toDouble() < v2.toDouble();

			return result;
		}

		QVariant ExprOperator::greaterthan(const QVariant& v1, const QVariant& v2)
		{
			QVariant result;

			if (v1.type() == QVariant::Type::String && v2.type() == QVariant::Type::String)
				result = v1.toString() > v2.toString();
			else if (v1.type() == QVariant::Int && v2.type() == QVariant::Type::Int)
				result = v1.toInt() > v2.toInt();
			else if (v1.canConvert(QVariant::Type::Double) && v1.canConvert(QVariant::Type::Double))
				result = v1.toDouble() > v2.toDouble();

			return result;
		}

		QVariant ExprOperator::equal(const QVariant& v1, const QVariant& v2)
		{
			QVariant result;

			if (v1.type() == QVariant::Type::String && v2.type() == QVariant::Type::String)
				result = v1.toString() == v2.toString();
			else if (v1.type() == QVariant::Int && v2.type() == QVariant::Type::Int)
				result = v1.toInt() == v2.toInt();
			else if (v1.canConvert(QVariant::Type::Double) && v1.canConvert(QVariant::Type::Double))
				result = std::fabs(v1.toDouble() - v2.toDouble()) < 0.001;

			return result;
		}

		QVariant ExprOperator::notequal(const QVariant& v1, const QVariant& v2)
		{
			QVariant result;

			if (v1.type() == QVariant::Type::String && v2.type() == QVariant::Type::String)
				result = v1.toString() != v2.toString();
			else if (v1.type() == QVariant::Int && v2.type() == QVariant::Type::Int)
				result = v1.toInt() != v2.toInt();
			else if (v1.canConvert(QVariant::Type::Double) && v1.canConvert(QVariant::Type::Double))
				result = std::fabs(v1.toDouble() - v2.toDouble()) > 0.001;

			return result;
		}

		QVariant ExprOperator::greaterequal(const QVariant& v1, const QVariant& v2)
		{
			QVariant result;

			if (v1.type() == QVariant::Type::String && v2.type() == QVariant::Type::String)
				result = v1.toString() >= v2.toString();
			else if (v1.type() == QVariant::Int && v2.type() == QVariant::Type::Int)
				result = v1.toInt() >= v2.toInt();
			else if (v1.canConvert(QVariant::Type::Double) && v1.canConvert(QVariant::Type::Double))
				result = v1.toDouble() >= v2.toDouble();

			return result;
		}

		QVariant ExprOperator::lessthanequal(const QVariant& v1, const QVariant& v2)
		{
			QVariant result;

			if (v1.type() == QVariant::Type::String && v2.type() == QVariant::Type::String)
				result = v1.toString() <= v2.toString();
			else if (v1.type() == QVariant::Int && v2.type() == QVariant::Type::Int)
				result = v1.toInt() <= v2.toInt();
			else if (v1.canConvert(QVariant::Type::Double) && v1.canConvert(QVariant::Type::Double))
				result = v1.toDouble() <= v2.toDouble();

			return result;
		}

		QVariant ExprOperator::logicaland(const QVariant& v1, const QVariant& v2)
		{
			QVariant result;

			if (v1.type() == QVariant::Type::Bool && v2.type() == QVariant::Type::Bool)
				result = v1.toBool() && v2.toBool();

			return result;
		}

		QVariant ExprOperator::logicalor(const QVariant& v1, const QVariant& v2)
		{
			QVariant result;

			if (v1.type() == QVariant::Type::Bool && v2.type() == QVariant::Type::Bool)
				result = v1.toBool() || v2.toBool();

			return result;
		}

		QVariant ExprOperator::plus(const QVariant& v1, const QVariant& v2)
		{
			QVariant result;

			if (v1.type() == QVariant::Type::String && v2.type() == QVariant::Type::String)
				result = v1.toString() + v2.toString();
			else if (v1.type() == QVariant::Int && v2.type() == QVariant::Type::Int)
				result = v1.toInt() + v2.toInt();
			else if (v1.canConvert(QVariant::Type::Double) && v1.canConvert(QVariant::Type::Double))
				result = v1.toDouble() + v2.toDouble();

			return result;
		}

		QVariant ExprOperator::minus(const QVariant& v1, const QVariant& v2)
		{
			QVariant result;

			if (v1.type() == QVariant::Int && v2.type() == QVariant::Type::Int)
				result = v1.toInt() - v2.toInt();
			else if (v1.canConvert(QVariant::Type::Double) && v1.canConvert(QVariant::Type::Double))
				result = v1.toDouble() - v2.toDouble();

			return result;
		}

		QVariant ExprOperator::times(const QVariant& v1, const QVariant& v2)
		{
			QVariant result;

			if (v1.type() == QVariant::Int && v2.type() == QVariant::Type::Int)
				result = v1.toInt() * v2.toInt();
			else if (v1.canConvert(QVariant::Type::Double) && v1.canConvert(QVariant::Type::Double))
				result = v1.toDouble() * v2.toDouble();

			return result;
		}

		QVariant ExprOperator::divide(const QVariant& v1, const QVariant& v2)
		{
			QVariant result;

			if (v1.type() == QVariant::Int && v2.type() == QVariant::Type::Int)
				result = v1.toInt() / v2.toInt();
			else if (v1.canConvert(QVariant::Type::Double) && v1.canConvert(QVariant::Type::Double))
				result = v1.toDouble() / v2.toDouble();

			return result;
		}

		QString ExprOperator::toString()
		{
			QString result;

			for (size_t i = 0; i < nodes_.size(); i++)
			{
				result = nodes_[i]->toString();
				if (i != nodes_.size() - 1)
				{
					switch (op_)
					{
					case OpType::Plus:
						result += " + ";
						break;
					case OpType::Minus:
						result += " - ";
						break;
					case OpType::Times:
						result += " * ";
						break;
					case OpType::Divide:
						result += " / ";
						break;
					}
				}
			}

			return result;
		}

		void ExprOperator::allVariables(QStringList& list)
		{
			for (auto expr : nodes_)
				expr->allVariables(list);
		}
	}
}
