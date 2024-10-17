#include "DPRCalculator.h"
#include "OPRCalculator.h"

using namespace Eigen;

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			DPRCalculator::DPRCalculator(OPRCalculator& opr) : opr_(opr)
			{
			}

			DPRCalculator::~DPRCalculator()
			{
			}

			int DPRCalculator::calcRowCount()
			{
				int rows = 0;
				for (auto m : opr_.dataModel()->matches())
				{
					if (m->hasExtraField(Alliance::Red, 1, BATotalPoints) && m->hasExtraField(Alliance::Blue, 1, BATotalPoints))
						rows += 2;
				}

				return rows;
			}

			double DPRCalculator::getOPRScore(std::shared_ptr<const DataModelMatch> m, Alliance a)
			{
				double score = 0.0;
				for (int slot = 1; slot <= 3; slot++)
				{
					QString tkey = m->team(a, slot);
					score += opr_[tkey];
				}

				return score;
			}

			bool DPRCalculator::buildTeamMatchMatrix(MatrixXd& mat, VectorXd& score)
			{
				int row = 0;

				mat.setZero(calcRowCount(), static_cast<int>(opr_.dataModel()->teams().size()));
				score.setZero(static_cast<int>(calcRowCount()));


				for (auto m : opr_.dataModel()->matches())
				{
					if (!m->hasExtraField(Alliance::Red, 1, BATotalPoints) || !m->hasExtraField(Alliance::Blue, 1, BATotalPoints))
						continue;

					Alliance c = Alliance::Red;
					Alliance sc = Alliance::Blue;
					for (int slot = 1; slot <= 3; slot++)
					{
						QString key = m->team(c, slot);
						int index = findTeamIndex(key);

						mat(row, index) = 1;
					}

					double oprscore = getOPRScore(m, sc);
					QVariant v = m->value(sc, 1, BATotalPoints);
					if (!v.isValid())
						continue;

					score(row) = v.toDouble() / oprscore * 3.0;
					row++;

					c = Alliance::Blue;
					sc = Alliance::Red;
					for (int slot = 1; slot <= 3; slot++)
					{
						QString key = m->team(c, slot);
						int index = findTeamIndex(key);

						mat(row, index) = 1;
					}

					oprscore = getOPRScore(m, sc);
					v = m->value(sc, 1, BATotalPoints);
					if (!v.isValid())
						continue;

					score(row) = v.toDouble() / oprscore * 3.0;
					row++;
				}

				return true;
			}

			int DPRCalculator::findTeamIndex(const QString& key)
			{
				if (team_to_index_.size() == 0)
				{
					int index = 0;
					for (auto team : opr_.dataModel()->teams())
					{
						team_to_index_.insert_or_assign(team->key(), index++);
					}
				}

				auto it = team_to_index_.find(key);
				if (it == team_to_index_.end())
					return -1;

				return it->second;
			}

			DPRCalculator::Error DPRCalculator::calc()
			{
				if (!opr_.hasData())
				{
					if (!opr_.calc())
						return Error::OPRCalculationFailed;
				}

				int rows = calcRowCount();
				if (rows == 0)
					return Error::NoTeams;

				MatrixXd m;
				VectorXd scores;

				if (!buildTeamMatchMatrix(m, scores))
					return Error::NoTeams;

				MatrixXd mt = m.transpose();
				MatrixXd rt = mt * scores;

				MatrixXd mp = mt * m;
				MatrixXd mpinv = mp.completeOrthogonalDecomposition().pseudoInverse();
				MatrixXd dpr = mpinv * rt;

				for (auto pair : team_to_index_)
				{
					dpr_[pair.first] = dpr(pair.second);

				}

				return Error::Success;
			}
		}
	}
}
