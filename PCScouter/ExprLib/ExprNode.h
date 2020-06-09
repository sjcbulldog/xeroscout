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

#include <QVariant>
#include <QStringList>

namespace xero
{
	namespace expr
	{
		class ExprContext;
		class Expr;

		class ExprNode
		{
		public:
			ExprNode(Expr* e) {
				expr_ = e;
			}

			virtual ~ExprNode() {
			}

			virtual QVariant eval(ExprContext& ctxt) = 0;
			virtual QString toString() = 0;
			virtual void allVariables(QStringList& list) = 0;

		protected:
			Expr* expr() {
				return expr_;
			}

		private:
			Expr* expr_;
		};
	}
}
