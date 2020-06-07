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
#include <vector>
#include <memory>

namespace xero
{
	namespace expr
	{
		class ExprOperator : public ExprNode
		{
		public:
			enum class OpType
			{
				Plus,
				Minus,
				Times,
				Divide,
			};

		public:
			ExprOperator(Expr* e, OpType op);
			virtual ~ExprOperator();

			virtual QVariant eval(ExprContext& ctxt);
			virtual QString toString();
			virtual void allVariables(QStringList& list);

			void addNode(std::shared_ptr<ExprNode> node) {
				nodes_.insert(nodes_.begin(), node);
			}

			int prec();

		private:
			static QVariant plus(const QVariant& v1, const QVariant& v2);
			static QVariant minus(const QVariant& v1, const QVariant& v2);
			static QVariant times(const QVariant& v1, const QVariant& v2);
			static QVariant divide(const QVariant& v1, const QVariant& v2);

		private:
			OpType op_;
			std::vector<std::shared_ptr<ExprNode>> nodes_;
		};
	}
}

