//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
// 

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
				static constexpr const char* BATotalPoints = "ba_totalPoints";

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
