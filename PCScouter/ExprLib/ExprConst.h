//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
// 

#pragma once
#include "ExprNode.h"

namespace xero
{
	namespace expr
	{
		class ExprConst : public ExprNode
		{
		public:
			ExprConst(Expr* e, const QVariant& v) : ExprNode(e) {
				value_ = v;
			}

			virtual ~ExprConst() {
			}

			virtual QVariant eval(ExprContext& ctxt) {
				return value_;
			}

			virtual QString toString() {
				return value_.toString();
			}

			virtual void allVariables(QStringList& list) {
			}

		private:
			QVariant value_;
		};
	}
}
