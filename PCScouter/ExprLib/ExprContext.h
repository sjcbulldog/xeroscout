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
#include <QVariant>
#include <memory>

namespace xero
{
	namespace expr
	{
		class ExprNode;

		class EXPRLIB_EXPORT ExprContext
		{
		public:
			ExprContext() {
			}

			virtual ~ExprContext() {
			}

			virtual bool isValidVariable(const QString&) {
				return false;
			}

			virtual QVariant getVariable(const QString& name) {
				QVariant v;
				return v;
			}

			virtual bool isValidFunction(const QString& name) {
				return false;
			}

			virtual QVariant evalFunction(const QString &name, std::vector<std::shared_ptr<ExprNode>> values) {
				QVariant v;
				return v;
			}

		private:
		};
	}
}
