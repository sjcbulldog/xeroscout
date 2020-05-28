#include "OPRCalculator.h"
#include <QFile>
#include <QTextStream>

using namespace Eigen;

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			OPRCalculator::OPRCalculator(std::shared_ptr<ScoutingDataModel> dm) {
				dm_ = dm;
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
					if (m->hasExtraField(Alliance::Red, BATotalPoints) && m->hasExtraField(Alliance::Blue, BATotalPoints))
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
					if (!m->hasExtraField(Alliance::Red, BATotalPoints) || !m->hasExtraField(Alliance::Blue, BATotalPoints))
						continue;

					Alliance c = Alliance::Red;
					for (int slot = 1; slot <= 3; slot++)
					{
						QString key = m->team(c, slot);
						int index = findTeamIndex(key);

						mat(row, index) = 1;
					}

					QVariant v = m->value(c, 1, BATotalPoints);
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

					v = m->value(c, 1, "ba_totalPoints");
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
				MatrixXd mpinv = mp.inverse();
				MatrixXd opr = mpinv * rt;

				for (auto pair : team_to_index_)
				{
					dm_->setTeamOPR(pair.first, opr(pair.second));
				}

				return true;
			}
		}
	}
}
