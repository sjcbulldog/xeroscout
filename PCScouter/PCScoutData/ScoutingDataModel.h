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

/// \file ScoutingDataModel.h
///       This file contains the top level data model for the scouting database

#pragma once

#include "pcscoutdata_global.h"
#include "RobotActivity.h"
#include "ScoutingForm.h"
#include "ScoutingDataSet.h"
#include "ScoutingDatabase.h"
#include "DataModelTeam.h"
#include "DataModelMatch.h"
#include "TabletIdentity.h"
#include "SyncHistoryRecord.h"
#include "PickListTranslator.h"
#include "GraphDescriptorCollection.h"
#include "GraphDescriptor.h"
#include "FieldRegion.h"
#include "SequencePattern.h"
#include "PickListEntry.h"
#include "RobotCapabilities.h"
#include <QString>
#include <QJsonDocument>
#include <QFile>
#include <QObject>
#include <QJsonObject>
#include <list>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			/// The data model for the scouting data
			class PCSCOUTDATA_EXPORT ScoutingDataModel : public QObject
			{
				friend class JSonDataModelConverter;

				Q_OBJECT

			public:
				///\brief the argument to the modelChanged signal.  Indicates the type of change
				enum class ChangeType {
					HistoryChanged,					///< the history of changes changed
					TabletListChanged,				///< the list of team or match scouting tablets was changed
					TeamAdded,						///< a new team was added to the data model
					TeamDataChanged,				///< the data associated with a team was changed
					TeamScoutingDataAdded,			///< team scouting data was added or replaced for a team
					TeamScoutingTabletChanged,		///< the tablet used for team scouting a team was changed
					MatchScoutingDataAdded,			///< match scouting data was added or replaced for a match
					MatchScoutingTabletChanged,		///< the tablet used for match scouting a match was changed
					TeamAddedToMatch,				///< added a team to a match
					MatchAdded,						///< a new match was added to the data model
					MatchDataChanged,				///< the data associated with a match was changed
					Reset,							///< the data model was reset
					ZebraDataAdded,					///< zebra data was added to the matches
					GraphDescriptor,				///< the graph descriptors were changed
					TeamSummaryFieldList,			///< the list of fields in the team summary changed
					DataSetColumnOrder,				///< the column order for a dataset
					MatchVideoAdded,				///< added match video links
					PickListTranslatorAdded,		///< added the pick list translator
					FieldRegionsAdded,				///< a field region was added
					FieldRegionRemoved,				///< a field region was removed
					FieldRegionRenamed,				///< a field retion was renamed
					ActivityAdded,					///< an activity was added
					ActivityRemoved,				///< an activity was removed
					ActivityChanged,				///< an activity was changed
					RulesChanged,					///< the rules for a data set changed
					PickListChanged,				///< the picklist changed
				};

			public:
				/// \brief create a new data model
				/// \param evkey the event Blue Alliance key
				/// \param evname the event name
				/// \param teamform the name of the file containing the team scouting form
				/// \param matchform the name of the file contining the match scouting form
				ScoutingDataModel(const QString& evkey, const QString& evname, const QDate& start, const QDate& end);

				/// \brief create a new data model
				ScoutingDataModel();

				/// \brief destroy this data model
				virtual ~ScoutingDataModel();

				void setInvertZebraData(bool b) {
					invert_zebra_data_ = b;
				}

				bool invertZebraData() {
					return invert_zebra_data_;
				}

				void getScoutingFormImages(QStringList& imlist);

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//
				// These method query the data model but do not change them.  They will never cause a modelChanged event.
				//
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				/// \brief returns the starting data for the event
				/// \return the starting data for the event
				const QDate& startDate() const {
					return start_date_;
				}

				/// \brief returns the starting data for the event
				/// \return the starting data for the event
				const QDate& endDate() const {
					return end_date_;
				}

				/// \brief return the Blue Alliance event key
				/// \returns the blue alliance event key
				const QString& evkey() const {
					return ev_key_;
				}

				/// \brief return the UUID for this data file
				/// \returns the UUID for this datafile
				const QUuid& uuid() const {
					return uuid_;
				}

				/// \brief return the graph view descriptors
				/// \returns the graph descriptors
				const GraphDescriptorCollection& graphDescriptors() const {
					return graph_descriptor_;
				}

				/// \brief returns the event name
				/// \returns the event name
				const QString& evname() const {
					return event_name_;
				}

				/// \brief returns the history of synchronizations made to the data model
				/// \returns a list of synchronizations made to the data model
				std::list<SyncHistoryRecord> history() const {
					std::list<SyncHistoryRecord> ret;

					for (auto h : history_)
						ret.push_back(h);

					return ret;
				}

				/// \brief returns the list of teams in the datamodel
				/// \returns the list of teams in the datamodel
				std::list<std::shared_ptr<const DataModelTeam>> teams() const {
					std::list<std::shared_ptr<const DataModelTeam>> list;

					for (auto t : teams_)
						list.push_back(t);

					return list;
				}

				/// \brief returns a shared DataModelTeam pointer given the key for the team
				/// \param key the Blue Alliance key for the desired team
				/// \returns a shared DataModelTeam pointer given the key for the team
				std::shared_ptr<const DataModelTeam> findTeamByKey(const QString& key) const {
					auto it = teams_by_key_.find(key);
					if (it == teams_by_key_.end())
						return nullptr;

					return it->second;
				}

				/// \brief find a team given its team number
				/// \param num the number of the team to find
				/// \returns a with with the given team number
				std::shared_ptr<const DataModelTeam> findTeamByNumber(int num) const {
					for (auto t : teams_) {
						if (t->number() == num)
							return t;
					}

					return nullptr;
				}

				/// \brief find a match given the Blue Alliance key for the match
				/// \param key the blue alliance key for the match
				/// returns a shared pointer to the match if a match with the given key was found, otherwise nullptr
				std::shared_ptr<const DataModelMatch> findMatchByKey(const QString& key) const {
					auto it = matches_by_key_.find(key);
					if (it == matches_by_key_.end())
						return nullptr;

					return it->second;
				}

				/// \brief return a list of the matches in the data model
				/// \returns a list of the matches in the data model
				std::list<std::shared_ptr<const DataModelMatch>> matches() const {
					std::list<std::shared_ptr<const DataModelMatch>> ret;

					for (auto m : matches_)
						ret.push_back(m);

					return ret;
				}

				/// \brief find a match given its type, set and match
				/// \param type the type of match (qm, qf, sf, f)
				/// \param set the set number
				/// \param match the match number
				/// \returns a pointer to the match if a match with the given type, set, and match was found, otherwise returns null
				std::shared_ptr<DataModelMatch> findMatchByTriple(const QString& type, int set, int match) const {
					for (auto m : matches_) {
						if (m->compType() == type && m->set() == set && m->match() == match)
							return m;
					}

					return nullptr;
				}

				/// \brief returns the set of tablets for match scouting
				/// \returns the set of tablets for match scouting
				const QStringList& matchTablets() {
					return match_tablets_;
				}

				/// \brief returns the set of tablets for teamform scouting
				/// \returns the set of tablets for teamform scouting
				const QStringList& teamTablets() {
					return team_tablets_;
				}

				/// \brief returns true if the data model has changed since it was last saved
				/// \returns true if the data model has changed since it was last saved
				bool isDirty() const {
					return dirty_;
				}

				/// \brief returns a pointer to the match scouting form
				/// \returns a pointer to the match scouting form
				std::shared_ptr<const ScoutingForm> matchScoutingForm() const {
					return match_scouting_form_;
				}

				/// \brief returns a pointer to the teamform scouting form
				/// \returns a pointer to the teamscouting form
				std::shared_ptr<const ScoutingForm> teamScoutingForm() const {
					return team_scouting_form_;
				}

				/// \brief returns all field names across the team scouting form, the match scouting form, and the blue alliance data
				/// \returns all field names ;
				std::vector<std::shared_ptr<FieldDesc>> getAllFields() const;

				/// \brief return feilds associated with a team
				/// \returns field names assocatied with a team
				std::vector<std::shared_ptr<FieldDesc>> getTeamFields() const;

				/// \brief returns fields that contain per match data 
				/// \returns fields that are valid per match data
				std::vector<std::shared_ptr<FieldDesc>> getMatchFields() const;

				/// \brief returns a field descriptor given a field name
				/// \returns a field descriptor given a field name
				std::shared_ptr<FieldDesc> getFieldByName(const QString& name) const;

				/// \brief return the extra team fields in the data model
				std::list<std::shared_ptr<FieldDesc>> teamExtraFields() const {
					return team_extra_fields_;
				}

				/// \brief return the extra match fields in the data model
				std::list<std::shared_ptr<FieldDesc>> matchExtraFields() const {
					return match_extra_fields_;
				}

				/// \brief return the team summary list of fields
				/// \returns the team summary list of fields
				const QStringList& teamSummaryFields() const {
					return team_summary_fields_;
				}

				/// \brief given a dataset name, return the column order
				/// \returns the column order for a dataset
				bool datasetColumnOrder(const QString& dsname, QByteArray &state, QByteArray &geom) const {
					auto it = dataset_column_order_.find(dsname);
					if (it == dataset_column_order_.end())
						return false;
					
					state = it->second.first;
					geom = it->second.second;
					return true;
				}

				/// \brief return the names of the datasets with order data stored
				/// \returns the names of the datasets with order data stored
				const QStringList datasetColumnOrderNames() const {
					QStringList list;

					for (auto pair : dataset_column_order_)
						list.push_back(pair.first);

					return list;
				}

				/// \brief return the pick list translator
				std::shared_ptr<const PickListTranslator> pickListTranslator() const {
					return pick_list_trans_;
				}

				std::vector<std::shared_ptr<const FieldRegion>> fieldRegions() const {
					std::vector<std::shared_ptr<const FieldRegion>> list;

					for (auto h : regions_)
						list.push_back(h);

					return list;
				}

				std::vector<std::shared_ptr<const RobotActivity>> activities() const {
					std::vector<std::shared_ptr<const RobotActivity>> list;

					for (auto p : activities_)
						list.push_back(p);

					return list;
				}

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//
				// These methods generate data sets.  The are a special case of the query methods in that they synthesis sets of data
				// from the datamodel.  These methods may have significant runtimes.
				//
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				/// \brief create a dataset for the matches in the datamodel
				/// Iterate over all of the matches and create a dataset.  The rows are the matches.  The columns are
				/// the scouting data for each match.  The scouting data is the data defined in the match scouting form
				/// as well as the data loaded for each match from the Blue Alliance if the match data has been imported.
				/// \param set the dataset to contain the data.  Any existing data will be cleared.
				void createMatchDataSet(ScoutingDataSet& set) ;

				/// \brief create a dataset for the pits in the datamodel.
				/// Iterate over all of the teams anad create a dataset for team scouting data.  The rows are the teams and
				/// the columns are the scouting data fields from the team scouting form
				/// \param set the dataset to contain the data.  Any existing data will be cleared.
				void createTeamDataSet(ScoutingDataSet& set) ;

				/// \brief create a dataset for a custom SQL query
				/// The rows and columns are defined by the results of the query
				/// \param set the dataset to contain the data.  Any existing data will be cleared.
				/// \param query the SQL query to execute
				/// \param error the error message if the SQL query was invalid
				bool createCustomDataSet(ScoutingDataSet& set, const QString& query, QString& error) ;

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//
				// These method change the datamodel and will generate modelChanged signals
				//
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				void addRobotCapability(const RobotCapabilities& cap) {
					robot_capablities_.push_back(cap);

					dirty_ = true;
					emitChangedSignal(ChangeType::PickListChanged);
				}

				const std::vector<RobotCapabilities>& robotCapablities() const {
					return robot_capablities_;
				}

				void resetPicklist() {
					picklist_ = original_picklist_;

					dirty_ = true;
					emitChangedSignal(ChangeType::PickListChanged);
				}

				void clearPickList() {
					picklist_.clear();
					original_picklist_.clear();
					robot_capablities_.clear();

					dirty_ = true;
					emitChangedSignal(ChangeType::PickListChanged);
				}

				void replacePickList(const std::vector<PickListEntry> &picklist) {
					picklist_ = picklist;

					dirty_ = true;
					emitChangedSignal(ChangeType::PickListChanged);
				}

				void addPickListEntry(const PickListEntry& entry) {
					original_picklist_.push_back(entry);

					dirty_ = true;
					emitChangedSignal(ChangeType::PickListChanged);
				}

				const std::vector<PickListEntry>& picklist() const {
					return picklist_;
				}

				void setRules(const QString& name, const std::list<std::shared_ptr<DataSetHighlightRules>>& rules) {
					rules_.insert_or_assign(name, rules);
					dirty_ = true;
					emitChangedSignal(ChangeType::RulesChanged);
				}

				bool addActivity(std::shared_ptr<const RobotActivity> p) {
					dirty_ = true;

					auto a = std::const_pointer_cast<RobotActivity>(p);
					activities_.push_back(a);

					emitChangedSignal(ChangeType::ActivityAdded);

					return true;
				}

				bool updatedActivity(std::shared_ptr<const RobotActivity> act) {
					auto it = std::find(activities_.begin(), activities_.end(), act);
					if (it == activities_.end())
						return false;

					auto a = std::const_pointer_cast<RobotActivity>(act);
					*a = *act;

					dirty_ = true;
					emitChangedSignal(ChangeType::ActivityChanged);

					return true ;
				}				

				void renameActivity(const QString &oldname, const QString& newname)
				{
					for (auto a : activities_)
					{
						if (a->name() == oldname)
						{
							a->setName(newname);
							emitChangedSignal(ChangeType::ActivityChanged);
							return;
						}
					}
				}

				void removeActivty(std::shared_ptr<const RobotActivity> p) {

					for (auto it = activities_.begin(); it != activities_.end(); it++)
					{
						auto act = *it;
						if (act->name() == p->name())
						{
							activities_.erase(it);
							dirty_ = true;
							emitChangedSignal(ChangeType::ActivityRemoved);

							break;
						}
					}
				}

				bool addFieldRegion(std::shared_ptr<FieldRegion> h) {
					for (auto r : regions_)
					{
						if (r->name() == h->name())
							return false;
					}

					dirty_ = true;
					regions_.push_back(h);
					emitChangedSignal(ChangeType::FieldRegionsAdded);

					return true;
				}

				void removeFieldRegion(std::shared_ptr<const FieldRegion> h) {
					auto it = std::find(regions_.begin(), regions_.end(), h);

					if (it != regions_.end())
					{
						regions_.erase(it);
						dirty_ = true;
						emitChangedSignal(ChangeType::FieldRegionRemoved);
					}
				}

				void renameFieldRegion(std::shared_ptr<const FieldRegion> h, const QString& newname) {
					auto it = std::find(regions_.begin(), regions_.end(), h);
					if (it == regions_.end())
						return;

					auto r = std::const_pointer_cast<FieldRegion>(h);
					r->rename(newname);

					dirty_ = true;
					emitChangedSignal(ChangeType::FieldRegionRenamed);
				}

				/// \brief set the scouting forms for the data model
				/// This can only be done once right after the data model is created.  Any subsequent call to this API will return false.
				/// \param teamform the scouting form for team (pit) scouting
				/// \param matchform the scouting form for match scouting
				/// \param dups a list of duplicate fields found between the two forms
				bool setScoutingForms(std::shared_ptr<ScoutingForm> teamform, std::shared_ptr<ScoutingForm> matchform, QStringList& dups);

				bool setScoutingForm(std::shared_ptr<ScoutingForm> form) {
					bool ret = false;

					QStringList dups;

					if (form->formType() == "team") {
						ret = setScoutingForms(form, nullptr, dups);
					}
					else {
						ret = setScoutingForms(nullptr, form, dups);
					}

					return ret;
				}

				/// \brief set the pick list tranlator data
				/// \param trans the pick list translator object
				void setPickListTranslator(std::shared_ptr<PickListTranslator> trans) {
					dirty_ = true;
					pick_list_trans_ = trans;
					emitChangedSignal(ChangeType::PickListTranslatorAdded);
				}

				/// \brief sets the extra field descriptors that are team specific
				/// \param fields the fields to add to the extra fields list
				void addTeamExtraFields(std::list<std::shared_ptr<FieldDesc>> fields) {
					for (auto f : fields)
					{
						auto oldf = getFieldByName(f->name());
						if (oldf == nullptr)
						{
							team_extra_fields_.push_back(f);
						}
						else
						{
							assert(oldf->type() == f->type());
						}
					}

					dirty_ = true;
					emitChangedSignal(ChangeType::TeamDataChanged);
				}

				/// <summary>
				/// Add a new data field to the team extra and add the field if it does not exist
				/// </summary>
				/// <param name="name"></param>
				/// <param name="value"></param>
				void addTeamExtraData(const QString &tkey, const QString& name, const QVariant& value);

				/// \brief set the OPR value for a team
				/// \param teamkey the key for the team to set OPR for
				/// \param opr the OPR value for the team
				void setTeamOPR(const QString& teamkey, double opr) {
					auto team = findTeamByKeyInt(teamkey);
					team->setOPR(opr);

					if (getFieldByName(DataModelTeam::OPRName) == nullptr)
						team_extra_fields_.push_back(std::make_shared<FieldDesc>(DataModelTeam::OPRName, FieldDesc::Type::Double, false));

					dirty_ = true;
					emitChangedSignal(ChangeType::TeamDataChanged);
				}

				void setTeamDPR(const QString& teamkey, double opr) {
					auto team = findTeamByKeyInt(teamkey);
					team->setDPR(opr);

					if (getFieldByName(DataModelTeam::DPRName) == nullptr)
						team_extra_fields_.push_back(std::make_shared<FieldDesc>(DataModelTeam::DPRName, FieldDesc::Type::Double, false));

					dirty_ = true;
					emitChangedSignal(ChangeType::TeamDataChanged);
				}

				/// \brief set the ranking data from the blue alliance.  
				/// This data is stored as the JSON object received from the blue alliance
				/// \param key the team key identifying the team of interest
				/// \param obj the ranking object data as a JSON object
				void setTeamRanking(const QString& key, const QJsonObject& obj) {
					auto team = findTeamByKeyInt(key);
					team->setRanking(obj);

					if (getFieldByName(DataModelTeam::RankName) == nullptr)
						team_extra_fields_.push_back(std::make_shared<FieldDesc>(DataModelTeam::RankName, FieldDesc::Type::Integer, false));

					dirty_ = true;
					emitChangedSignal(ChangeType::TeamDataChanged);
				}

				/// \brief add data field descriptors aligned with the extra match data in the record
				/// This method takes the data values in the set and deduces the field types to add to the match field descriptors
				/// \param the data
				void addMatchExtraDataFields(ScoutingDataMapPtr data);

				/// \brief sets the extra field descriptors that are match specific
				void addMatchExtraFields(std::list<std::shared_ptr<FieldDesc>> fields) {
					for (auto f : fields)
					{
						auto oldf = getFieldByName(f->name());
						if (oldf == nullptr)
						{
							match_extra_fields_.push_back(f);
						}
					}

					dirty_ = true;
					emitChangedSignal(ChangeType::MatchDataChanged);
				}

				void addMatchExtraData(const QString& key, Alliance c, int slot, ScoutingDataMapPtr data) {
					auto m = findMatchByKeyInt(key);
					m->addExtraData(c, slot, data);

					dirty_ = true;
					emitChangedSignal(ChangeType::TeamDataChanged);
				}

				/// \brief set the various graph views
				/// \param grarray an array of group view descriptors as JSON
				/// \param err any returned error message
				/// \returns true if the info is updated
				bool setGraphViews(const QJsonArray& grarray, QString &err) {
					return graph_descriptor_.load(grarray, err);
				}

				/// \brief set the list of fields that appear on the team summary
				/// \param list the list of fields that appear on the team summary
				void setTeamSummaryFields(const QStringList& list) {
					team_summary_fields_ = list;
					dirty_ = true;

					emitChangedSignal(ChangeType::TeamSummaryFieldList);
				}

				/// \brief set the column order for a dataset
				/// \param dsname the name of the dataset
				/// \param list the column order for the columns
				void setDatasetColumnOrder(const QString& dsname, const QByteArray &state, const QByteArray &geom) {
					dataset_column_order_.insert_or_assign(dsname, std::make_pair(state, geom));
					dirty_ = true;

					emitChangedSignal(ChangeType::DataSetColumnOrder);
				}

				void clearDatasetColumnOrder() {
					dataset_column_order_.clear();
					dirty_ = true;

					emitChangedSignal(ChangeType::DataSetColumnOrder);
				}

				/// \brief add a team to a match.
				/// This is used when constructing a match and therefore teams are expected to be added in the
				/// position order for the match.
				/// \param matchkey the Blue Alliance key for the match to add the team to
				/// \param c the color of the alliance (red or blue)
				/// \param teamkey the Blue Alliance key for the team
				/// \parar number the team number
				/// \returns false if the match does not exist, or if it has teams already otherwise true
				bool addTeamToMatch(const QString& matchkey, Alliance c, int slot, const QString& teamkey, int number) {
					auto dm = findMatchByKeyInt(matchkey);
					if (dm == nullptr)
						return false;

					dm->addTeam(c, slot, teamkey, number);

					dirty_ = true;
					emitChangedSignal(ChangeType::TeamAddedToMatch);

					return true;
				}

				/// \brief assign a tablet to a team for team scouting
				/// \param key the Blue Alliance key for the team
				/// \param the tablet name for the tablet to assign
				/// \returns true if the team key indicates a valid team, false otherwise
				bool assignTeamTablet(const QString& key, const QString& tablet)
				{
					auto team = findTeamByKeyInt(key);
					if (team == nullptr)
						return false;

					team->setTablet(tablet);

					dirty_ = true;

					emitChangedSignal(ChangeType::TeamScoutingTabletChanged);

					return true;
				}

				/// \brief set the scouting data for a team
				/// \param key the Blue Alliance key for the team
				/// \param data the scouting data for the team
				/// \retursn true if the team key indicates a valid team, false otherwise
				bool setTeamScoutingData(const QString& key, ScoutingDataMapPtr data, bool replace = false)
				{
					auto team = findTeamByKeyInt(key);
					if (team == nullptr)
						return false;

					bool ret = team->setTeamScoutingData(data, replace);
					if (ret)
					{
						dirty_ = true;
						emitChangedSignal(ChangeType::TeamScoutingDataAdded);
					}

					return ret;
				}

				/// \brief assign a tablet to a match
				/// \param key the Blue Alliance key for a match
				/// \param color the color of the alliance for this tablet
				/// \param slot the position (1, 2, 3) for the tablet
				/// \param tablet the tablet name to assign
				/// \returns false if the match key is invalid, or the slot is not between 1 - 3
				bool assignMatchTablet(const QString& key, Alliance color, int slot, const QString& tablet)
				{
					auto match = findMatchByKeyInt(key);
					if (match == nullptr)
						return false;

					if (slot < 1 || slot > 3)
						return false;

					match->setTablet(color, slot, tablet);

					dirty_ = true;

					emitChangedSignal(ChangeType::MatchScoutingTabletChanged);

					return true;
				}

				/// \brief assign scouting data to a match
				/// \param key the Blue Alliance key for a match
				/// \param color the color of the alliance for this tablet
				/// \param slot the position (1, 2, 3) for the tablet
				/// \param data the scouting data to assign
				/// \param false if true, replace all scouting data with this one entry
				/// \returns false if the match key is invalid, or the slot is not between 1 - 3
				bool setMatchScoutingData(const QString& key, Alliance color, int slot, ScoutingDataMapPtr data, bool replace = false)
				{
					bool ret;

					auto match = findMatchByKeyInt(key);
					if (match == nullptr)
						return false;

					if (slot < 1 || slot > 3)
						return false;

					ret = match->setScoutingData(color, slot, data, replace);
					if (ret)
					{
						dirty_ = true;
						emitChangedSignal(ChangeType::MatchScoutingDataAdded);
					}

					return ret;
				}

				/// \brief set the lists of tablets to use for scouting
				/// This method should only be called on a data model that does not have
				/// team tablets or match tablets assigned.  This method is used for
				/// constructing a data model.
				/// \returns true if sucessful and false if the team or match tablet scouting list where not empty
				bool setTabletLists(const QStringList& teams, const QStringList& matches) {
					if (team_tablets_.size() > 0 || match_tablets_.size() > 0)
						return false;

					team_tablets_ = teams;
					match_tablets_ = matches;

					dirty_ = true;
					emitChangedSignal(ChangeType::TabletListChanged);

					return true;
				}

				/// \brief add a new team to the data model
				/// \param key the Blue Alliance key for this team
				/// \param number the team number for the team
				/// \param the nickname for teh team
				/// \returns a shared pointer to the team data model object that was added
				std::shared_ptr<DataModelTeam> addTeam(const QString& key, int number, const QString &nick, const QString& name, const QString &city, const QString &state, const QString &country) {
					auto t = findTeamByKeyInt(key);
					if (t != nullptr) {
						if (t->name() != name || t->number() != number) {
							t->setName(name);
							t->setNumber(number);

							dirty_ = true;
							emitChangedSignal(ChangeType::TeamDataChanged);
						}
						return t;
					}

					t = std::make_shared<DataModelTeam>(key, number, nick, name, city, state, country);
					teams_.push_back(t);
					teams_by_key_.insert_or_assign(key, t);

					dirty_ = true;
					emitChangedSignal(ChangeType::TeamAdded);

					return t;
				}

				/// \brief add a new match to the data model.
				/// If a match with the given key already exists, update the information for the match if necessary and
				/// return a shared pointer to the match.  Otherwise, create the match and added it.  Return a shared pointer
				/// to the new match.
				/// \param key the Blue Alliance key for the match
				/// \param comp the type of match (qm, qf, sf, f)
				/// \param set the set number
				/// \param match the match number
				/// \param the expected time for the match
				/// \returns a shared pointer to the match
				std::shared_ptr<DataModelMatch> addMatch(const QString& key, const QString& comp, int set, int match, int etime) {
					auto m = findMatchByKeyInt(key);
					if (m != nullptr)
					{
						if (m->compType() != comp || m->set() != set || m->match() != match)
						{
							m->setCompType(comp);
							m->setSet(set);
							m->setMatch(match);
							dirty_ = true;

							emitChangedSignal(ChangeType::MatchDataChanged);
						}
						return m;
					}

					m = std::make_shared<DataModelMatch>(key, comp, set, match, etime);
					matches_.push_back(m);
					matches_by_key_.insert_or_assign(key, m);

					dirty_ = true;
					emitChangedSignal(ChangeType::MatchAdded);

					return m;
				}

				/// \brief add zebra data to data model
				/// \param key the match key for the match
				/// \param obj the JSON object containing zebra data (in blue alliance format)
				void assignZebraData(const QString& key, const QJsonObject& obj) {
					auto m = findMatchByKeyInt(key);
					m->setZebra(obj);

					dirty_ = true;
					emitChangedSignal(ChangeType::ZebraDataAdded);
				}

				void assignVideos(const QString& key, const std::list<std::pair<QString, QString>>& videos) {
					auto m = findMatchByKeyInt(key);
					m->setVideos(videos);

					dirty_ = true;
					emitChangedSignal(ChangeType::MatchVideoAdded);
				}

				void updateGraphDescriptor(const GraphDescriptor& desc) {
					graph_descriptor_.update(desc);

					dirty_ = true;
					emitChangedSignal(ChangeType::GraphDescriptor);
				}

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//
				// These method are file I/O methods for the data model
				//
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				/// \brief returns the filename associated with the data model
				/// \returns the filename associted with the data models
				const QString& filename() {
					return filename_;
				}

				/// \brief set the filename for saving the datamodel
				/// \param filename the filename for the data model
				void setFilename(const QString& filename) {
					filename_ = filename;
				}

				/// \brief resets the filename to empty
				void resetFilename() {
					filename_.clear();
				}

				/// \brief save the data model to the default file
				/// If there is no default file, return false
				/// \returns true if the data was saved, otherwise returns false
				bool save() {

					if (filename_.length() == 0)
						return false;

					QJsonDocument doc = generateAllJSONDocument();
					QFile file(filename_);

					if (!file.open(QIODevice::WriteOnly))
						return false;

					QByteArray data;

					if (filename_.endsWith(".evd"))
					{
						data = doc.toJson(QJsonDocument::JsonFormat::Indented);
					}
					else
					{
						data = doc.toJson(QJsonDocument::JsonFormat::Indented);
					}

					if (file.write(data) != data.size())
					{
						file.close();
						return false;
					}

					file.close();
					dirty_ = false;
					return true;
				}

				/// \brief save the data model to the file name given
				/// \returns true if the data was saved, otherwise false
				bool save(const QString& filename) {
					filename_ = filename;
					return save();
				}

				/// \brief load the datamodel from the file given
				/// Note, the modelChanged signal is disabled during the load operation as this would
				/// trigger signals for each data model loaded from file.  It is restored when the load
				/// operation is complete.
				/// \returns true if the data model was loaded successfully
				bool load(const QString& filename);

				/// \brief load the scouting data from the file given
				/// Note, the modelChanged signal is disabled during the load operation as this would
				/// trigger signals for each data model loaded from file.  It is restored when the load
				/// operation is complete.
				/// \returns true if the data model was loaded successfully
				bool loadScoutingDataJSON(const QString& filename);

				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//
				// These method are misc methods
				//
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				void addImageView(const QString& username, const QString &imname) {
					image_view_list_.push_back(std::make_pair(username, imname));
				}

				const std::list<std::pair<QString, QString>> & imageViews() const {
					return image_view_list_;
				}

				void changeTeamData(const QString& tkey, const QString& field, const QVariant& value);
				void changeMatchData(const QString &mkey, const QString& tkey, const QString& field, const QVariant& value);


				/// \brief remove all but the latest scouting data
				void removeOldScoutingData();

				/// \brief assign tablets to matches for match scouting
				void assignMatches();

				/// \brief assign tablets to team for team scouting
				void assignTeams();

				/// \brief given a team key, return the team number
				/// \param key the Blue Alliance key for the team
				/// \returns the team number
				int teamKeyToNumber(const QString& key);

				QJsonDocument generateZebraData(const QStringList& matchkeys);
				QJsonDocument generateMatchDetailData(const QStringList& matchkeys);

				/// \brief generate a JSON document that represents all of the data in the data model
				/// \returns a json documentthat represents all of the data in the data model
				QJsonDocument generateAllJSONDocument();

				/// \brief generate a JSON document that represents all of the data in the data model except scouting data
				/// \returns a json documentthat represents all of the data in the data model except scouting data
				QJsonDocument generateCoreJSONDocument(bool extra = true);

				/// \brief generate a JSON document that represents the scouting data in the data model for a specific tablet
				/// \param sender the identity of the tablet requesting the data, incorporated as part of the data sent from the tablet to the central, can be null
				/// \param the tablet we want data for
				/// \returns a JSON document that represents the scouting data in the data model
				QJsonDocument generateScoutingData(const TabletIdentity* sender, const QString& tablet);

				bool loadMatchDetailData(const QJsonDocument& doc);
				bool loadZebraData(const QJsonDocument& doc);

				/// \brief load all of the json data in the document into the data model.
				/// The JSON data is expected to be of type 'all' which means both core data and scouting data.
				/// Note if this method returns false, partial data may have loaded into the data model.  Use save()
				/// and load() to save and restore the model if necessary.
				/// \param doc the json document to load
				/// \param error if this method returns false, the errors that occurred are stored in this list
				/// \returns true if the data was loaded into the data model, false otherwise
				bool loadAllJSON(const QJsonDocument& doc, QStringList& error);

				/// \brief load the json data in the document into the data model
				/// The JSON data is expected to be of type 'core' which means just core data.
				/// Note if this method returns false, partial data may have loaded into the data model.  Use save()
				/// and load() to save and restore the model if necessary.
				/// \param doc the json document to load
				/// \param error if this method returns false, the errors that occurred are stored in this list
				/// \returns true if the data was loaded into the data model, false otherwise
				bool loadCoreJSON(const QJsonDocument& doc, QStringList& error);

				/// \brief load the json data in the document into the data model
				/// The JSON data is expected to be of type 'scouting' which means just scouting data
				/// Note if this method returns false, partial data may have loaded into the data model.  Use save()
				/// and load() to save and restore the model if necessary.
				/// \param doc the json document to load
				/// \param error if this method returns false, the errors that occurred are stored in this list
				/// \returns true if the data was loaded into the data model, false otherwise
				bool loadScoutingDataJSON(const QJsonDocument& doc, TabletIdentity& tablet, QString& error, QStringList& pits, QStringList& matches);

				/// \brief peek into the json document and get the UUID for the dataset
				/// \param doc the document to examine
				/// \param uuid the returned UUID
				/// \returns true if the UUID was found, otherwise false
				bool peekUUID(const QJsonDocument& doc, QUuid& uuid);

				/// \brief reset the data in the data model
				/// This destroys all data in the data model.  Wse with care
				void reset() {
					filename_.clear();

					ev_key_.clear();
					event_name_.clear();

					uuid_ = QUuid();

					match_scouting_form_ = nullptr;
					team_scouting_form_ = nullptr;
					teams_.clear();
					matches_.clear();

					registered_tablets_.clear();

					team_tablets_.clear();
					match_tablets_.clear();

					history_.clear();

					dirty_ = false;
					emitChangedSignal(ChangeType::Reset);
				}

				/// \brief generate randome data to fill all scouting data in the model.
				/// Used primarimaly for testing the application.  The mod parameter is used to
				/// tell which items get data.  A value of 1 will generate data for all items.  A value
				/// of two will generate data for 1/2.  3 for 1/3, etc.
				/// \param profile the data profile for generating random data
				/// \param redmax the maximum match number that will get random data
				/// \param bluemax the maximum match number that will get random data
				void generateRandomScoutingData(int redmax, int bluemax);

				/// \brief remove any scouting data associated with the given tablet
				/// \param tablet the tablet we want to remove data for
				void removeScoutingData(const QString& tablet);

				/// \brief return the the registered tablets assocaited with the data model.
				/// This is the list of the identities of every tablet that has ever synchronized with the data model.
				/// \returns the list of tablet identities
				const std::list<TabletIdentity> registeredTablets() const {
					return registered_tablets_;
				}

				/// \brief register the given tablet with the data model
				/// \param id the tablet identity to register
				void registerTablet(const TabletIdentity& id) {
					registered_tablets_.push_back(id);
				}

				void addHistoryRecord(const SyncHistoryRecord& rec) {
					history_.push_back(rec);

					dirty_ = true;
					emitChangedSignal(ChangeType::HistoryChanged);
				}

				/// \brief if signals are enabled, signal any listener that the data model changed
				/// \param type the type of the change made
				void emitChangedSignal(ChangeType type) {
					if (emit_changes_)
					{
						auto db = ScoutingDatabase::getInstance();
						db->clearAllTables();

						emit modelChanged(type);
					}
				}

				void clearDatabaseTables() {
					auto db = ScoutingDatabase::getInstance();
					db->clearAllTables();
				}

			signals:
				void modelChanged(ChangeType type);

			protected:
				/// \brief returns a shared DataModelTeam pointer given the key for the team
				/// \param key the Blue Alliance key for the desired team
				/// \returns a shared DataModelTeam pointer given the key for the team
				std::shared_ptr<DataModelTeam> findTeamByKeyInt(const QString& key) {
					for (auto t : teams_) {
						if (t->key() == key)
							return t;
					}

					return nullptr;
				}

				std::shared_ptr<DataModelMatch> findMatchByKeyInt(const QString& key) {
					for (auto m : matches_) {
						if (m->key() == key)
							return m;
					}

					return nullptr;
				}

			private:
				void addRulesToDataSet(ScoutingDataSet &set);
				void addTemporaryFieldDesc();

				void incrMatchTabletIndex(int& index) {
					index++;
					if (index == match_tablets_.size())
						index = 0;
				}

				void incrTeamTabletIndex(int& index) {
					index++;
					if (index == team_tablets_.size())
						index = 0;
				}

				std::shared_ptr<DataModelMatch> findMatch(const QString& comp, int set, int match);

				void processDataSetAlliance(ScoutingDataSet& set, std::shared_ptr<DataModelMatch> m, Alliance c) const;

			private:
				QString filename_;

				bool dirty_;

				QUuid uuid_;
				QString ev_key_;
				QString event_name_;

				QStringList match_tablets_;
				std::list<std::shared_ptr<DataModelMatch>> matches_;
				std::map<QString, std::shared_ptr<DataModelMatch>> matches_by_key_;
				std::shared_ptr<ScoutingForm> match_scouting_form_;
				std::list<std::shared_ptr<FieldDesc>> match_extra_fields_;

				QStringList team_tablets_;
				std::list<std::shared_ptr<DataModelTeam>> teams_;
				std::map<QString, std::shared_ptr<DataModelTeam>> teams_by_key_;
				std::shared_ptr<ScoutingForm> team_scouting_form_;
				std::list<std::shared_ptr<FieldDesc>> team_extra_fields_;

				std::list<TabletIdentity> registered_tablets_;

				std::list<SyncHistoryRecord> history_;

				bool emit_changes_;

				GraphDescriptorCollection graph_descriptor_;

				QStringList team_summary_fields_;

				std::map<QString, std::pair<QByteArray, QByteArray>> dataset_column_order_;

				QDate start_date_;
				QDate end_date_;

				std::shared_ptr<PickListTranslator> pick_list_trans_;

				std::vector<std::shared_ptr<FieldRegion>> regions_;

				std::vector<std::shared_ptr<RobotActivity>> activities_;

				std::map<QString, std::list<std::shared_ptr<DataSetHighlightRules>>> rules_;

				std::vector<PickListEntry> picklist_;
				std::vector<PickListEntry> original_picklist_;

				std::vector<RobotCapabilities> robot_capablities_;

				std::list<std::pair<QString, QString>> image_view_list_;

				bool invert_zebra_data_;
			};
		}
	}
}
