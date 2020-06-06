#include "DataGenerator.h"
#include <QCoreApplication>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			DataGenerator::DataGenerator(std::shared_ptr<ScoutingDataModel> dm, int year, int maxred, int maxblue, int maxteam)
			{
				dm_ = dm;
				year_ = year;
				maxred_ = maxred;
				maxblue_ = maxblue;
				maxteam_ = maxteam;
				match_ptr_ = nullptr;
				team_ptr_ = nullptr;

				loadGenerator();
			}

			DataGenerator::~DataGenerator()
			{
				if (lib_ != nullptr)
					delete lib_;
			}

			void DataGenerator::run()
			{
				dm_->blockSignals(true);
				generateTeams();
				generateMatches();
				dm_->blockSignals(false);
				dm_->emitChangedSignal(ScoutingDataModel::ChangeType::MatchDataChanged);
				dm_->emitChangedSignal(ScoutingDataModel::ChangeType::TeamDataChanged);
			}

			void DataGenerator::generateMatches()
			{
				for (auto m : dm_->matches())
				{
					auto data = generate(m, dm_->matchScoutingForm());
					if (data.size() != 6)
						continue;

					int index = 0;
					Alliance c = Alliance::Red;
					for (int slot = 1; slot <= 3; slot++)
					{
						dm_->setMatchScoutingData(m->key(), c, slot, data[index++]);
					}

					c = Alliance::Blue;
					for (int slot = 1; slot <= 3; slot++)
					{
						dm_->setMatchScoutingData(m->key(), c, slot, data[index++]);
					}
				}
			}

			void DataGenerator::generateTeams()
			{
				for (auto t : dm_->teams())
				{
					auto data = generate(t, dm_->teamScoutingForm());
					if (data != nullptr)
						dm_->setTeamScoutingData(t->key(), data, true);
				}
			}

			ScoutingDataMapPtr DataGenerator::generate(std::shared_ptr<const DataModelTeam> team, std::shared_ptr<const ScoutingForm> form)
			{
				if (team_ptr_ == nullptr)
					return nullptr;

				return (*team_ptr_)(team, form);
			}

			std::vector<ScoutingDataMapPtr> DataGenerator::generate(std::shared_ptr<const DataModelMatch> match, std::shared_ptr<const ScoutingForm> form)
			{
				std::vector<ScoutingDataMapPtr> ret;
				if (match_ptr_ == nullptr)
					return ret;

				return (*match_ptr_)(match, form);
			}

#ifdef _MSC_VER
			//
			// Windows, MSVC
			//
			const char *match = "?generateMatch@@YA?AV?$vector@V?$shared_ptr@V?$map@VQString@@VQVariant@@U?$less@VQString@@@std@@V?$allocator@U?$pair@$$CBVQString@@VQVariant@@@std@@@4@@std@@@std@@V?$allocator@V?$shared_ptr@V?$map@VQString@@VQVariant@@U?$less@VQString@@@std@@V?$allocator@U?$pair@$$CBVQString@@VQVariant@@@std@@@4@@std@@@std@@@2@@std@@V?$shared_ptr@$$CBVDataModelMatch@datamodel@scouting@xero@@@2@V?$shared_ptr@$$CBVScoutingForm@datamodel@scouting@xero@@@2@@Z";
			const char* team = "?generateTeam@@YA?AV?$shared_ptr@V?$map@VQString@@VQVariant@@U?$less@VQString@@@std@@V?$allocator@U?$pair@$$CBVQString@@VQVariant@@@std@@@4@@std@@@std@@V?$shared_ptr@$$CBVDataModelTeam@datamodel@scouting@xero@@@2@V?$shared_ptr@$$CBVScoutingForm@datamodel@scouting@xero@@@2@@Z";
#endif

#ifdef __GNUC__
			//
			// Linux
			//
			const char* match = "TODO";
			const char* team = "TODO";
#endif

#ifdef __clang__
			//
			// MacOS
			//
			const char* match = "TODO";
			const char* team = "TODO";
#endif

			void DataGenerator::loadGenerator()
			{
				QString dir = QCoreApplication::applicationDirPath() + "/Generator" + QString::number(year_) + ".dll";
				lib_ = new QLibrary(dir);

				lib_->load();
				if (lib_->isLoaded())
				{
					match_ptr_ = reinterpret_cast<generate_match>(lib_->resolve(match));
					team_ptr_ = reinterpret_cast<generate_team>(lib_->resolve(team));

					if (match_ptr_ != nullptr && team_ptr_ != nullptr)
					{
						emit logMessage("Loaded generator library '" + dir + "'");
					}
					else
					{
						emit logMessage("Library '" + dir + "' was not loaded sucessfully");

						delete lib_;
						lib_ = nullptr;

						match_ptr_ = nullptr;
						team_ptr_ = nullptr;
					}
				}
			}

		}
	}

}