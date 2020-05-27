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
			class PCSCOUTDATA_EXPORT OPRCalculator
			{
			public:
				OPRCalculator(std::shared_ptr<ScoutingDataModel> dm);
				virtual ~OPRCalculator();

				bool calc();

			private:
				int calcRowCount();
				int findTeamIndex(const QString &key);
				bool buildTeamMatchMatrix(Eigen::MatrixXd& m, Eigen::VectorXd& score);

			private:
				std::map<QString, double> opr_;
				std::shared_ptr<ScoutingDataModel> dm_;
				std::map<QString, int> team_to_index_;
			};
		}
	}
}
