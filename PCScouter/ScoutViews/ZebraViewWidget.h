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
#include "ViewBase.h"
#include "PathFieldView.h"
#include "DataModelMatch.h"
#include "TimeBoundWidget.h"
#include "ZebraDataInfoExtractor.h"
#include "RobotTrack.h"
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
			class SCOUTVIEWS_EXPORT ZebraViewWidget : public QWidget, public ViewBase
			{
			public:
				ZebraViewWidget(QWidget* parent);
				virtual ~ZebraViewWidget();

				void clearView();
				void refreshView();

				PathFieldView* field() {
					return field_;
				}

				void setDataModel(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> model) {
					ViewBase::setDataModel(model);
					if (mode_ != Mode::Uninitialized)
						createPlot();

					if (field_ != nullptr) {
						for (auto h : model->fieldRegions())
							field_->addHighlight(h);
					}
				}

			protected:
				void showEvent(QShowEvent*) override;

			private:
				static const constexpr double AnimationTimeStep = 0.1;

				enum class Mode
				{
					Uninitialized,
					SingleMatch,
					SingleTeam,
				};

				enum class HighlightType
				{
					Circle,
					Rectangle,
					Polygon
				};

			private:
				void detailItemChanged(QListWidgetItem* item);

				void matchesSelected(bool checked);
				void robotSelected(bool checked);
				void comboxChanged(int index);
				void modeChanged(int index);

				void updateComboBoxMatch();
				void updateComboBoxTeam();

				void createPlot();
				void createPlotMatch(const QString& key);
				void createPlotTeam(const QString& key);

				void rangeChanged(double minv, double maxv);

				void getTimes(const QJsonArray& array, std::shared_ptr<xero::scouting::datamodel::RobotTrack> track);

				QColor matchRobotColor(xero::scouting::datamodel::Alliance c, int slot);
				bool extractOneAlliance(const QJsonArray& arr, int index, std::shared_ptr<xero::scouting::datamodel::RobotTrack> track);

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
				void addHighlight(xero::scouting::datamodel::Alliance a, HighlightType ht);
				void removeHighlight(const QString& name);
				void areaSelected(const QRectF& area, xero::scouting::datamodel::Alliance a, HighlightType ht);
				void polygonSelected(const std::vector<QPointF>& points, xero::scouting::datamodel::Alliance a);

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
				QComboBox* box_;
				QRadioButton* matches_;
				QRadioButton* robot_;
				PathFieldView* field_;
				TimeBoundWidget* slider_;
				QPushButton* detail_;
				QSplitter* vertical_;
				QSplitter* horizontal_;
				QWidget* hholder_;
				QTreeWidget* info_;
				QListWidget* list_;
				QCheckBox* all_;
				QComboBox* mode_select_;

				QTimer* animation_timer_;
				double animation_time_;
				double animation_mult_;

				Mode mode_;
				std::vector<TrackEntry> entries_;

				QPoint menu_point_;

				QMetaObject::Connection field_connect_;
			};
		}
	}
}

