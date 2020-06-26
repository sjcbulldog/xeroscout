#pragma once

#include "ExprNode.h"
#include <memory>
#include <vector>

namespace xero
{
	namespace expr
	{
		class ExprFunction : public ExprNode
		{
		public:
			ExprFunction(Expr *e, const QString& name, std::vector<std::shared_ptr<ExprNode>> args);
			virtual ~ExprFunction();

			virtual QVariant eval(ExprContext& ctxt);
			virtual QString toString();
			virtual void allVariables(QStringList& list);

		private:
			QString name_;
			std::vector<std::shared_ptr<ExprNode>> args_;
		};
	}
}
