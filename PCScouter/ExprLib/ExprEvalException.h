#pragma once

#include <stdexcept>

namespace xero
{
	namespace expr
	{
		class ExprEvalException : public std::runtime_error
		{
		public:
			ExprEvalException(const char* what) : std::runtime_error(what)
			{
			}
		};
	}
}

