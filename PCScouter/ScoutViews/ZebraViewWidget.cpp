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
//

#include "ZebraViewWidget.h"
#include "RobotTrack.h"
#include "CircleFieldRegion.h"
#include "RectFieldRegion.h"
#include "PolygonFieldRegion.h"
#include "DataModelBuilder.h"
#include <QBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <functional>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			ZebraViewWidget::ZebraViewWidget(QWidget* parent, GameFieldManager &mgr, const QString &year, 
						const QString &name, PathFieldView::ViewMode vm) : FieldBasedWidget(mgr, parent), ViewBase(name)
			{
				QVBoxLayout* vlay = new QVBoxLayout();
				setLayout(vlay);

				selector_ = new MatchTeamSelector(dataModel(), this);
				vlay->addWidget(selector_);
				connect(selector_, &MatchTeamSelector::matchSelected, this, &ZebraViewWidget::matchesRobotsSelected);
				connect(selector_, &MatchTeamSelector::robotSelected, this, &ZebraViewWidget::matchesRobotsSelected);
				connect(selector_, &MatchTeamSelector::selectedItemChanged, this, &ZebraViewWidget::comboxChanged);

				vertical_ = new QSplitter(Qt::Orientation::Horizontal, this);
				vlay->addWidget(vertical_);
				QSizePolicy p = QSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
				vertical_->setSizePolicy(p);

				auto f = new PathFieldView(vertical_);
				setField(f, year);
				connect(field(), &PathFieldView::showContextMenu, this, &ZebraViewWidget::fieldContextMenu);
				vertical_->addWidget(field());
				field()->setViewMode(vm);

				horizontal_ = new QSplitter(Qt::Orientation::Vertical, vertical_);
				vertical_->addWidget(horizontal_);

				QWidget* top_right = new QWidget(horizontal_);
				QVBoxLayout *vlay2 = new QVBoxLayout();
				top_right->setLayout(vlay2);
				horizontal_->addWidget(top_right);

				all_ = new QCheckBox(top_right);
				vlay2->addWidget(all_);
				connect(all_, &QCheckBox::stateChanged, this, &ZebraViewWidget::checkChanged);

				list_ = new QListWidget(top_right);
				vlay2->addWidget(list_);
				list_->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
				connect(list_, &QListWidget::itemChanged, this, &ZebraViewWidget::detailItemChanged);

				info_ = new QTreeWidget(horizontal_);
				info_->setHeaderHidden(true);
				info_->setColumnCount(1);
				horizontal_->addWidget(info_);

				slider_ = new TimeBoundWidget(0.0, 135.0, this);
				vlay->addWidget(slider_);
				connect(slider_, &TimeBoundWidget::rangeChanged, this, &ZebraViewWidget::rangeChanged);
				connect(slider_, &TimeBoundWidget::changeAnimationState, this, &ZebraViewWidget::sliderChangedAnimationState);
				connect(slider_, &TimeBoundWidget::currentTimeChanged, this, &ZebraViewWidget::animationSetTime);

				animation_timer_ = nullptr;

				if (vm == PathFieldView::ViewMode::Replay)
				{
					//
					// Setup the slider for replay mode.
					//
					slider_->setReplayMode(true);
					animationSetTime(slider_->rangeStart());
				}
			}

			ZebraViewWidget::~ZebraViewWidget()
			{
			}

			void ZebraViewWidget::fieldContextMenu(QPoint pt)
			{
				menu_point_ = pt;
				QAction* act;

				if (field()->viewMode() == PathFieldView::ViewMode::Replay)
				{
					QMenu* menu = new QMenu("Field");

					act = menu->addAction("Show Defense");
					act->setCheckable(true);
					if (field()->showDefense())
						act->setChecked(true);
					else
						act->setChecked(false);
					connect(act, &QAction::triggered, this, &ZebraViewWidget::defenseToggled);

					menu->exec(pt);
				}
			}

			void ZebraViewWidget::defenseToggled()
			{
				field()->setShowDefense(!field()->showDefense());
			}

			void ZebraViewWidget::sliderChangedAnimationState(bool state, double mult)
			{
				animation_mult_ = mult;

				if (state)
				{
					if (animation_timer_ == nullptr)
					{
						animationSetTime(slider_->rangeStart());

						animation_timer_ = new QTimer(this);
						animation_timer_->setInterval(std::chrono::milliseconds(static_cast<int>(AnimationTimeStep * 1000)));
						(void)connect(animation_timer_, &QTimer::timeout, this, &ZebraViewWidget::animationProc);
						animation_timer_->start();
					}
				}
				else
				{
					if (animation_timer_ != nullptr)
					{
						animation_timer_->stop();
						delete animation_timer_;
						animation_timer_ = nullptr;
					}
				}
			}

			void ZebraViewWidget::animationSetTime(double t)
			{
				animation_time_ = t;
				for (TrackEntry &te : entries_) {
					te.track()->setCurrentTime(t);
				}
				slider_->setCurrentTime(t);

				slider_->update();
				field()->update();
			}

			void ZebraViewWidget::animationProc()
			{
				animation_time_ += animation_mult_ * AnimationTimeStep ;
				if (animation_time_ > slider_->rangeEnd())
					animation_time_ = slider_->rangeStart();

				animationSetTime(animation_time_);
			}

			void ZebraViewWidget::checkChanged(int state)
			{
				static bool inside = false;

				if (inside)
					return;

				inside = true;
				Qt::CheckState st = static_cast<Qt::CheckState>(state);
				if (st != Qt::CheckState::PartiallyChecked)
				{
					for (int i = 0; i < list_->count(); i++)
					{
						auto item = list_->item(i);
						item->setCheckState(st);
					}
				}
				inside = false;
			}

			void ZebraViewWidget::detailItemChanged(QListWidgetItem* item)
			{
				int entindex = item->data(Qt::UserRole).toInt();
				assert(entindex >= 0 && entindex < entries_.size());

				if (item->checkState() == Qt::CheckState::Checked)
				{
					if (!entries_[entindex].enabled())
					{
						field()->addTrack(entries_[entindex].track());
						entries_[entindex].setEnabled(true);
					}
				}
				else if (item->checkState() == Qt::CheckState::Unchecked)
				{
					if (entries_[entindex].enabled())
					{
						field()->removeTrack(entries_[entindex].track());
						entries_[entindex].setEnabled(false);
					}
				}
				field()->update();

				bool allon = true;
				bool alloff = true;

				for (int i = 0; i < list_->count(); i++)
				{
					Qt::CheckState st = list_->item(i)->checkState();
					if (st == Qt::CheckState::Checked)
						alloff = false;
					else if (st == Qt::CheckState::Unchecked)
						allon = false;
				}

				all_->blockSignals(true);
				if (allon)
				{
					all_->setCheckState(Qt::CheckState::Checked);
				}
				else if (alloff)
				{
					all_->setCheckState(Qt::CheckState::Unchecked);
				}
				else
				{
					all_->setCheckState(Qt::CheckState::PartiallyChecked);
				}
				all_->blockSignals(false);
			}

			void ZebraViewWidget::rangeChanged(double minv, double maxv)
			{
				for (auto t : field()->tracks())
				{
					t->setRange(slider_->rangeStart(), slider_->rangeEnd());
				}

				updatePerformance(true);
				field()->update();
			}

			void ZebraViewWidget::clearView()
			{
				selector_->clear();
				field()->clearTracks();
				entries_.clear();
			}

			void ZebraViewWidget::refreshView()
			{
				if (dataModel() != nullptr)
				{
					field()->clearHighlights();
					for (auto h : dataModel()->fieldRegions())
						field()->addHighlight(h);

					createPlot();
				}
			}

			QColor ZebraViewWidget::matchRobotColor(xero::scouting::datamodel::Alliance c, int slot)
			{
				QColor ret(0, 0, 0, 255);

				if (c == Alliance::Red)
				{
					switch (slot)
					{
					case 1:
						ret = QColor(255, 0, 0, 255);
						break;
					case 2:
						ret = QColor(128, 0, 0, 255);
						break;
					case 3:
						ret = QColor(255, 128, 128, 255);
						break;
					}
				}
				else
				{
					switch (slot)
					{
					case 1:
						ret = QColor(0, 0, 255, 255);
						break;
					case 2:
						ret = QColor(128, 128, 255, 255);
						break;
					case 3:
						ret = QColor(50, 168, 158, 255);
						break;
					}
				}

				return ret;
			}

			void ZebraViewWidget::resetAnimation()
			{
				if (field()->viewMode() == PathFieldView::ViewMode::Replay)
				{
					animationSetTime(slider_->minimum());
					if (animation_timer_ != nullptr)
					{
						animation_timer_->stop();
						delete animation_timer_;
						animation_timer_ = nullptr;
					}
				}
			}

			//
			// This is called when the matches radio button changes.  We only process the
			// event when checked is true meaning the user has asked for matches
			//
			void ZebraViewWidget::matchesRobotsSelected()
			{
				resetAnimation();
				setNeedRefresh();
				createPlot();
			}

			void ZebraViewWidget::setKey(const QString& key)
			{
				if (dataModel()->findMatchByKey(key))
				{
					selector_->setMatchKey(key);
				}
				else if (dataModel()->findTeamByKey(key))
				{
					selector_->setTeamKey(key);
				}
			}

			void ZebraViewWidget::comboxChanged(const QString &key)
			{
				resetAnimation();

				if (key.length() > 0)
				{
					setNeedRefresh();
					createPlot();
					field()->update();
				}
			}

			std::shared_ptr<RobotTrack> ZebraViewWidget::createTrack(const QString& mkey, const QString& tkey)
			{
				auto t = dataModel()->findTeamByKey(tkey);
				auto m = dataModel()->findMatchByKey(mkey);

				QString title;
				Alliance c;
				int slot;

				m->teamToAllianceSlot(tkey, c, slot);

				if (selector_->isMatchesSelected())
				{
					title = QString::number(t->number());
				}
				else
				{
					title = m->title(true);
				}

				auto track = DataModelBuilder::createTrack(dataModel(), mkey, tkey, 27.0);
				if (track == nullptr)
					return nullptr;

				if (selector_->isRobotsSelected() && c == Alliance::Blue)
				{
					track->transform(54.0, 27.0);
					c = Alliance::Red;
					slot = 1;
				}

				track->setTitle(title);
				track->setColor(matchRobotColor(c, slot));
				track->setAlliance(c);

				TrackEntry te(mkey, tkey, track);
				entries_.push_back(te);

				field()->addTrack(track);
				track->setRange(slider_->rangeStart(), slider_->rangeEnd());

				return track;
			}

			//
			// Called to create a plot from the ground up with everything visible
			//
			void ZebraViewWidget::createPlot()
			{
				if (selector_->isMatchesSelected())
				{
					createPlotMatch(selector_->currentKey());
				}
				else
				{
					createPlotTeam(selector_->currentKey());
				}

				all_->blockSignals(true);
				all_->setCheckState(Qt::CheckState::Checked);
				all_->blockSignals(false);
			}


			//
			// Called when we want to create a new zebra plot based on the a given match
			//
			void ZebraViewWidget::createPlotMatch(const QString& mkey)
			{
				if (!needsRefresh())
					return;

				field()->clearTracks();
				entries_.clear();

				auto m = dataModel()->findMatchByKey(mkey);
				if (m == nullptr)
					return;

				Alliance c = Alliance::Red;
				for (int i = 1; i <= 3; i++)
				{
					QString tkey = m->team(c, i);
					createTrack(mkey, tkey);
				}
				c = Alliance::Blue;
				for (int i = 1; i <= 3; i++)
				{
					QString tkey = m->team(c, i);
					createTrack(mkey, tkey);
				}

				updateDisplay();
			}

			void ZebraViewWidget::createPlotTeam(const QString& tkey)
			{
				if (!needsRefresh())
					return;

				entries_.clear();
				field()->clearTracks();

				for (auto m : dataModel()->matches())
				{
					if (!m->teamIsInAlliance(tkey))
						continue;

					createTrack(m->key(), tkey);
				}

				updateDisplay();
			}

			void ZebraViewWidget::updateDisplay()
			{
				updateDetail();
				updatePerformance(false);
				updateSlider();
			}

			void ZebraViewWidget::updateDetail()
			{
				list_->clear();

				for (int i = 0; i < entries_.size(); i++)
				{
					const TrackEntry& entry = entries_[i];
					QListWidgetItem* item;

					if (selector_->isMatchesSelected())
					{
						auto team = dataModel()->findTeamByKey(entry.teamKey());
						item = new QListWidgetItem(QString::number(team->number()) + " - " + team->nick());
					}
					else
					{
						auto m = dataModel()->findMatchByKey(entry.matchKey());
						item = new QListWidgetItem(m->title());
					}
					item->setData(Qt::UserRole, QVariant(i));

					if (entry.track() != nullptr)
						item->setCheckState(Qt::CheckState::Checked);
					else
						item->setCheckState(Qt::CheckState::Unchecked);

					list_->addItem(item);
				}
			}

			void ZebraViewWidget::updatePerformance(bool flush)
			{
				info_->clear();
				for (TrackEntry& te : entries_)
				{
					auto t = dataModel()->findTeamByKey(te.teamKey());
					auto m = dataModel()->findMatchByKey(te.matchKey());

					QTreeWidgetItem* item = new QTreeWidgetItem(info_);
					if (selector_->isMatchesSelected())
					{
						item->setText(0, t->title());
					}
					else
					{
						item->setText(0, m->title());
					}

					if (flush)
						te.info().flush();

					QString tt;
					ScoutingDataMapPtr data = te.info().characterize();
					QTreeWidgetItem* info;
					for (auto pair : *data)
					{
						info = new QTreeWidgetItem(item);
						QString value = QString::number(pair.second.toDouble(), 'f', 2);
						QString entry = pair.first + " = " + value;
						info->setText(0, entry);

						if (tt.length() > 0)
							tt += "\n";
						tt += entry;
					}

					item->setToolTip(0, tt);
				}

				info_->resizeColumnToContents(0);
				info_->resizeColumnToContents(1);
			}

			void ZebraViewWidget::updateSlider()
			{
				double minv = std::numeric_limits<double>::max();
				double maxv = std::numeric_limits<double>::min();

				for (const TrackEntry& te : entries_)
				{
					double mymin = te.track()->time(0);
					if (mymin < minv)
						minv = mymin;

					double mymax = te.track()->time(te.track()->timeCount() - 1);
					if (mymax > maxv)
						maxv = mymax;
				}

				slider_->setMinimum(minv);
				slider_->setMaximum(maxv);
				slider_->setRangeStart(minv);
				slider_->setRangeEnd(maxv);
			}
		}
	}
}

