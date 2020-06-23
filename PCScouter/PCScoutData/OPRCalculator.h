//
// Copyright(C) 2020 by Jack (Butch) Griffin
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see < https://www.gnu.org/licenses/>.
//
//
//
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
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
				OPRCalculator(std::shared_ptr<ScoutingDataModel> dm, const QString& field);
				virtual ~OPRCalculator();

				bool calc();
				bool hasData() {
					return opr_.size() > 0;
				}

				double operator[](const QString& key) {
					auto it = opr_.find(key);
					assert(it != opr_.end());

					return it->second;
				}

				std::shared_ptr<ScoutingDataModel> dataModel() {
					return dm_;
				}

				const QString& field() const {
					return field_;
				}

			private:
				int calcRowCount();
				int findTeamIndex(const QString &key);
				bool buildTeamMatchMatrix(Eigen::MatrixXd& m, Eigen::VectorXd& score);

			private:
				QString field_;
				std::map<QString, double> opr_;
				std::shared_ptr<ScoutingDataModel> dm_;
				std::map<QString, int> team_to_index_;
			};
		}
	}
}
