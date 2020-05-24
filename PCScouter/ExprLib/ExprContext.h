#pragma once

#include "exprlib_global.h"
#include <QVariant>

namespace xero
{
	namespace expr
	{
		class EXPRLIB_EXPORT ExprContext
		{
		public:
			ExprContext() {
			}

			virtual ~ExprContext() {
			}

			virtual bool isValidVariable(const QString&) = 0;
			virtual QVariant getVariable(const QString& name) = 0;

		private:
		};
	}
}
