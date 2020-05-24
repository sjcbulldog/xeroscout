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
