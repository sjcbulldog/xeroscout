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


