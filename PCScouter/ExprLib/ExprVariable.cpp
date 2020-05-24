#include "ExprVariable.h"
#include "ExprContext.h"

namespace xero
{
	namespace expr
	{
		ExprVariable::ExprVariable(Expr* e, const QString& name) : ExprNode(e)
		{
			name_ = name;
		}

		ExprVariable::~ExprVariable()
		{
		}

		QVariant ExprVariable::eval(ExprContext& ctxt)
		{
			return ctxt.getVariable(name_);
		}

		QString ExprVariable::toString()
		{
			return name_;
		}

		void ExprVariable::allVariables(QStringList& list)
		{
			if (!list.contains(name_))
				list.push_back(name_);
		}
	}
}
