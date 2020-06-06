#pragma once

#include "pcscoutdata_global.h"
#include "ScoutingDataModel.h"
#include <QObject>
#include <QLibrary>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class PCSCOUTDATA_EXPORT DataGenerator : public QObject
			{
				Q_OBJECT

			public:
				DataGenerator(std::shared_ptr<ScoutingDataModel> dm, int year, int maxred, int maxblue, int maxteams);
				virtual ~DataGenerator();

				void run();
				bool isValid() {
					return match_ptr_ != nullptr && team_ptr_ != nullptr;
				}

			signals:
				void logMessage(const QString& msg);

			private:
				void generateTeams();
				void generateMatches();
				void loadGenerator();

				ScoutingDataMapPtr generate(std::shared_ptr<const xero::scouting::datamodel::DataModelTeam> team, std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form);
				std::vector<ScoutingDataMapPtr> generate(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> team, std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form);

			private:
				typedef std::vector<xero::scouting::datamodel::ScoutingDataMapPtr>(*generate_match)(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> match,
					std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form);

				typedef xero::scouting::datamodel::ScoutingDataMapPtr(*generate_team)(std::shared_ptr<const xero::scouting::datamodel::DataModelTeam> team,
					std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form);

			private:
				std::shared_ptr<ScoutingDataModel> dm_;
				int year_;
				int maxred_;
				int maxblue_;
				int maxteam_;
				QLibrary* lib_;

				generate_match match_ptr_;
				generate_team team_ptr_;
			};
		}
	}
}
