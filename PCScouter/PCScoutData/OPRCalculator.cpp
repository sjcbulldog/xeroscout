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

#include "OPRCalculator.h"


using namespace Eigen;

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			OPRCalculator::OPRCalculator(std::shared_ptr<ScoutingDataModel> dm, const QString &field) {
				dm_ = dm;
				field_ = field;
			}

			OPRCalculator::~OPRCalculator()
			{
			}

			int OPRCalculator::findTeamIndex(const QString& key)
			{
				if (team_to_index_.size() == 0)
				{
					int index = 0;
					for (auto team : dm_->teams())
					{
						team_to_index_.insert_or_assign(team->key(), index++);
					}
				}

				auto it = team_to_index_.find(key);
				if (it == team_to_index_.end())
					return -1;

				return it->second;
			}

			int OPRCalculator::calcRowCount()
			{
				int rows = 0;
				for (auto m : dm_->matches())
				{
					if (m->hasExtraField(Alliance::Red, 1, field_) && m->hasExtraField(Alliance::Blue, 1, field_))
						rows += 2;
				}

				return rows;
			}

			bool OPRCalculator::buildTeamMatchMatrix(MatrixXd& mat, VectorXd& score)
			{
				int row = 0;

				mat.setZero(calcRowCount(), static_cast<int>(dm_->teams().size()));
				score.setZero(static_cast<int>(calcRowCount()));


				for (auto m : dm_->matches())
				{
					if (!m->hasExtraField(Alliance::Red, 1, field_) || !m->hasExtraField(Alliance::Blue, 1, field_))
						continue;

					Alliance c = Alliance::Red;
					for (int slot = 1; slot <= 3; slot++)
					{
						QString key = m->team(c, slot);
						int index = findTeamIndex(key);

						mat(row, index) = 1;
					}

					QVariant v = m->value(c, 1, field_);
					if (!v.isValid())
						v = QVariant(0);

					score(row) = v.toInt();
					row++;

					c = Alliance::Blue;
					for (int slot = 1; slot <= 3; slot++)
					{
						QString key = m->team(c, slot);
						int index = findTeamIndex(key);

						mat(row, index) = 1;
					}

					v = m->value(c, 1, field_);
					if (!v.isValid())
						v = QVariant(0);
					score(row) = v.toInt();

					row++;
				}

				return true;
			}

			bool OPRCalculator::calc()
			{
				if (dm_ == nullptr)
					return false;

				int rows = calcRowCount();
				if (rows == 0)
					return false;

				MatrixXd m;
				VectorXd scores;

				if (!buildTeamMatchMatrix(m, scores))
					return false;

				MatrixXd mt = m.transpose();
				MatrixXd rt = mt * scores;

				MatrixXd mp = mt * m;
				MatrixXd mpinv = mp.completeOrthogonalDecomposition().pseudoInverse();
				MatrixXd opr = mpinv * rt;

				for (auto pair : team_to_index_)
				{
					opr_[pair.first] = opr(pair.second);
				}

				return true;
			}
		}
	}
}
