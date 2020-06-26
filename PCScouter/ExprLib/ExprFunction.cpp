#include "ExprFunction.h"
#include "ExprContext.h"

namespace xero
{
	namespace expr
	{
		ExprFunction::ExprFunction(Expr* e, const QString& name, std::vector<std::shared_ptr<ExprNode>> args) : ExprNode(e)
		{
			name_ = name;
			args_ = args;
		}

		ExprFunction::~ExprFunction()
		{
		}

		QVariant ExprFunction::eval(ExprContext& ctxt)
		{
			return ctxt.evalFunction(name_, args_);
		}

		QString ExprFunction::toString()
		{
			QString ret = name_ + "(";
			for (int i = 0; i < args_.size(); i++)
			{
				ret += args_[i]->toString();
				if (i != args_.size() - 1)
					ret += ", ";
			}
			ret += ")";

			return ret;
		}

		void ExprFunction::allVariables(QStringList& list)
		{
			for (auto arg : args_)
				arg->allVariables(list);
		}
	}
}
