#include "Analysis2020.h"
#include "InputData.h"

namespace xero
{
	namespace analysis2020
	{
		Analysis2020::Analysis2020(InputData& data) : data_(data)
		{
		}

		Analysis2020::~Analysis2020()
		{
		}

		void Analysis2020::run()
		{
			sanityCheckData();
		}
	}
}
