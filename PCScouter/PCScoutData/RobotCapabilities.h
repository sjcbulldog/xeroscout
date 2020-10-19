#pragma once

#include <QString>
#include <QStringList>
#include <map>
#include <vector>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			typedef std::vector<std::pair<int, double>> Distribution;

			class RobotCapabilities
			{
			public:
				RobotCapabilities(int team) {
					team_ = team;
				}

				virtual ~RobotCapabilities() {
				}

				int team() const {
					return team_;
				}

				const std::map<QString, double>& doubleParams() const {
					return double_params_;
				}

				const std::map<QString, Distribution>& distParams() const {
					return dist_params_;
				}

				void addDoubleParam(const QString& name, double value) {
					double_params_.insert_or_assign(name, value);
				}

				void addDistParam(const QString& name, Distribution& dist) {
					dist_params_.insert_or_assign(name, dist);
				}

				QStringList doubleColumnNames() const {
					QStringList list;

					for (auto pair : double_params_)
						list.push_back(pair.first);

					return list;
				}

				QStringList distColumnNames() const {
					QStringList list;

					for (auto pair : dist_params_)
						list.push_back(pair.first);

					return list;
				}

			private:
				int team_;
				std::map<QString, double> double_params_;
				std::map<QString, Distribution> dist_params_;
			};
		}
	}
}
