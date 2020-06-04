#pragma once

namespace xero
{
	namespace analysis2020
	{
		class InputData;

		class Analysis2020
		{
		public:
			Analysis2020(InputData &data);
			virtual ~Analysis2020();

			void run();

		private:
			void sanityCheckData();

		private:
			int team_;
			InputData& data_;
		};
	}
}

