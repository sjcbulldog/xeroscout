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

#pragma once

#include "exprlib_global.h"
#include "ExprContext.h"
#include "ExprOperator.h"
#include "ExprNode.h"
#include <QVariant>
#include <QString>
#include <stack>

namespace xero
{
	namespace expr
	{
		class ExprNode;
		class ExprOperator;
		class ExprContext;

		class EXPRLIB_EXPORT Expr
		{
		public:
			Expr();
			virtual ~Expr();

			QString toString() const;

			bool parse(ExprContext &context, const QString& text, QString& err);

			QStringList allVariables() const;

			QVariant eval(ExprContext&) const;

		private:
			std::shared_ptr<ExprNode> parseSubExpr(ExprContext& context, const QString& txt, int& index, QString& err);
			std::shared_ptr<ExprNode> parseNode(ExprContext& context, const QString& txt, int& index, QString& err);
			std::shared_ptr<ExprOperator> parseOper(ExprContext& context, const QString& txt, int& index, QString& err);
			void skipSpaces(const QString& txt, int& index);

			void reduce(std::stack<std::shared_ptr<ExprNode>>& operands, std::stack<std::shared_ptr<ExprOperator>>& operators);
			void printStacks(std::stack<std::shared_ptr<ExprNode>>& operands, std::stack<std::shared_ptr<ExprOperator>>& operators);

		private:
			std::shared_ptr<ExprNode> top_;
		};
	}
}