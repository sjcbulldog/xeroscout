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

#include "scoutviews_global.h"
#include "MatchTeamSelector.h"
#include "ViewBase.h"
#include "PathFieldView.h"
#include "DataModelMatch.h"
#include "TimeBoundWidget.h"
#include "ZebraDataInfoExtractor.h"
#include "RobotTrack.h"
#include "GameFieldManager.h"
#include "FieldBasedWidget.h"
#include <QWidget>
#include <QComboBox>
#include <QRadioButton>
#include <QPushButton>
#include <QTableWidget>
#include <QTreeWidget>
#include <QSplitter>
#include <QListWidget>
#include <QCheckBox>
#include <QTimer>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT ZebraViewWidget : public FieldBasedWidget, public ViewBase
			{
			public:
				ZebraViewWidget(QWidget* parent, GameFieldManager &mgr, const QString &year, const QString &name, PathFieldView::ViewMode vm);
				virtual ~ZebraViewWidget();

				void clearView();
				void refreshView();

				void setDataModel(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> model) {
					ViewBase::setDataModel(model);

					if (field() != nullptr && model != nullptr)
					{
						for (auto h : model->fieldRegions())
							field()->addHighlight(h);
					}

					selector_->setDataModel(model);

					if (model != nullptr)
						createPlot();
				}

				void setKey(const QString& key);

			private:
				static const constexpr double AnimationTimeStep = 0.1;

			private:
				void detailItemChanged(QListWidgetItem* item);
				void resetAnimation();

				void matchesRobotsSelected();
				void comboxChanged(const QString& key);
				void modeChanged(int index);

				void createPlot();
				void createPlotMatch(const QString& key);
				void createPlotTeam(const QString& key);

				void rangeChanged(double minv, double maxv);

				QColor matchRobotColor(xero::scouting::datamodel::Alliance c, int slot);

				void updateDisplay();
				void updateDetail();
				void updateSlider();
				void updatePerformance(bool flush);
				void checkChanged(int state);

				void sliderChangedAnimationState(bool state, double mult);
				void animationProc();
				void animationSetTime(double t);

				void fieldContextMenu(QPoint pt);
				void defenseToggled();

				std::shared_ptr<xero::scouting::datamodel::RobotTrack> createTrack(const QString& mkey, const QString& tkey);

			private:

				class TrackEntry
				{
				public:
					TrackEntry(const QString& mkey, const QString& tkey, std::shared_ptr<xero::scouting::datamodel::RobotTrack> track) : zinfo_(track)
					{
						mkey_ = mkey;
						tkey_ = tkey;
						track_ = track;
						enabled_ = true;
					}

					virtual ~TrackEntry() {
					}
					
					bool enabled() const {
						return enabled_;
					}

					void setEnabled(bool b) {
						enabled_ = b;
					}

					const QString& matchKey() const {
						return mkey_;
					}

					const QString& teamKey() const {
						return tkey_;
					}

					std::shared_ptr<xero::scouting::datamodel::RobotTrack> track() const {
						return track_;
					}

					xero::scouting::datamodel::ZebraDataInfoExtractor& info() {
						return zinfo_;
					}

				private:
					QString mkey_;
					QString tkey_;
					bool enabled_;
					xero::scouting::datamodel::ZebraDataInfoExtractor zinfo_;
					std::shared_ptr<xero::scouting::datamodel::RobotTrack> track_;
				};

			private:
				MatchTeamSelector* selector_;
				TimeBoundWidget* slider_;
				QPushButton* detail_;
				QSplitter* vertical_;
				QSplitter* horizontal_;
				QWidget* hholder_;
				QTreeWidget* info_;
				QListWidget* list_;
				QCheckBox* all_;

				QTimer* animation_timer_;
				double animation_time_;
				double animation_mult_;

				std::vector<TrackEntry> entries_;

				QPoint menu_point_;

				QMetaObject::Connection field_connect_;
			};
		}
	}
}

