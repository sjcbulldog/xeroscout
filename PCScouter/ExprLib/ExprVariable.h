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
		class ExprVariable : public ExprNode
		{
		public:
			ExprVariable(Expr* e, const QString& name);
			virtual ~ExprVariable();

			virtual QVariant eval(ExprContext& ctxt);
			virtual QString toString();
			virtual void allVariables(QStringList& list);

		private:
			QString name_;
		};
	}
}


