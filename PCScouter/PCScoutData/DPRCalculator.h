#pragma once

#include "pcscoutdata_global.h"
#include "ScoutingDataModel.h"
#include <Eigen/Dense>
#include <QString>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class OPRCalculator;

			class PCSCOUTDATA_EXPORT DPRCalculator
			{
			public:
				enum class Error
				{
					OPRCalculationFailed,
					NoTeams,
					Success
				};
			public:
				DPRCalculator(OPRCalculator &opr);
				virtual ~DPRCalculator();

				Error calc();

				double operator[](const QString& key) {
					auto it = dpr_.find(key);
					assert(it != dpr_.end());

					return it->second;
				}

			private:
				static constexpr const char* BATotalPoints = "ba_totalPoints";

			private:
				int calcRowCount();
				int findTeamIndex(const QString& key);
				bool buildTeamMatchMatrix(Eigen::MatrixXd& m, Eigen::VectorXd& score);
				double getOPRScore(std::shared_ptr<const DataModelMatch>, Alliance a);

			private:
				OPRCalculator& opr_;
				std::map<QString, double> dpr_;
				std::map<QString, int> team_to_index_;
			};
		}
	}

}
