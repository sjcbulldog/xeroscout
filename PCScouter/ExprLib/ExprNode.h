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
