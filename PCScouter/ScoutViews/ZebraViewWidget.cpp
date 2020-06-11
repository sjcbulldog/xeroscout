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

#include "ZebraViewWidget.h"
#include "RobotTrack.h"
#include <QBoxLayout>
#include <QTableWidget>
#include <QHeaderView>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			ZebraViewWidget::ZebraViewWidget(QWidget* parent) : QWidget(parent), ViewBase("ZebraViewWidget")
			{
				QVBoxLayout* vlay = new QVBoxLayout();
				setLayout(vlay);

				QWidget* top = new QWidget(this);
				vlay->addWidget(top);

				QHBoxLayout* hlay = new QHBoxLayout();
				top->setLayout(hlay);

				matches_ = new QRadioButton("Match", top);
				hlay->addWidget(matches_);
				(void)connect(matches_, &QRadioButton::toggled, this, &ZebraViewWidget::matchesSelected);

				robot_ = new QRadioButton("Robot", top);
				hlay->addWidget(robot_);
				(void)connect(robot_, &QRadioButton::toggled, this, &ZebraViewWidget::robotSelected);

				box_ = new QComboBox(top);
				hlay->addWidget(box_);
				(void)connect(box_, static_cast<void (QComboBox::*)(int index)>(&QComboBox::currentIndexChanged), this, &ZebraViewWidget::comboxChanged);
				QSizePolicy p(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);
				box_->setSizePolicy(p);

				vertical_ = new QSplitter(Qt::Orientation::Horizontal, this);
				vlay->addWidget(vertical_);
				p = QSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
				vertical_->setSizePolicy(p);

				field_ = new PathFieldView(vertical_);
				vertical_->addWidget(field_);

				horizontal_ = new QSplitter(Qt::Orientation::Vertical, vertical_);
				vertical_->addWidget(horizontal_);

				list_ = new QListWidget(horizontal_);
				horizontal_->addWidget(list_);

				info_ = new QTreeWidget(horizontal_);
				horizontal_->addWidget(info_);

				slider_ = new TimeBoundWidget(0.0, 135.0, this);
				vlay->addWidget(slider_);
				connect(slider_, &TimeBoundWidget::rangeChanged, this, &ZebraViewWidget::rangeChanged);

				matches_->setChecked(true);
				matchesSelected(true);
			}

			ZebraViewWidget::~ZebraViewWidget()
			{
			}

			void ZebraViewWidget::showDetailMatch(const QString& key)
			{
				list_->clear();

				auto m = dataModel()->findMatchByKey(key);

				Alliance c;
				c = Alliance::Red;
				for (int i = 1; i <= 3; i++)
				{
					QString tkey = m->team(c, i);
					auto team = dataModel()->findTeamByKey(tkey);
					QListWidgetItem* item = new QListWidgetItem(QString::number(team->number()) + " - " + team->nick());
					item->setData(Qt::UserRole, tkey);
					Qt::ItemFlags f = Qt::ItemFlag::ItemIsUserCheckable | Qt::ItemFlag::ItemIsEnabled;
					item->setFlags(f);

					if (keys_.contains(tkey))
						item->setCheckState(Qt::CheckState::Checked);
					else
						item->setCheckState(Qt::CheckState::Unchecked);

					list_->addItem(item);
				}

				c = Alliance::Blue;
				for (int i = 1; i <= 3; i++)
				{
					QString tkey = m->team(c, i);
					auto team = dataModel()->findTeamByKey(tkey);
					QListWidgetItem* item = new QListWidgetItem(QString::number(team->number()) + " - " + team->nick());
					item->setData(Qt::UserRole, tkey);
					Qt::ItemFlags f = Qt::ItemFlag::ItemIsUserCheckable | Qt::ItemFlag::ItemIsEnabled;
					item->setFlags(f);

					if (keys_.contains(tkey))
						item->setCheckState(Qt::CheckState::Checked);
					else
						item->setCheckState(Qt::CheckState::Unchecked);

					list_->addItem(item);
				}

				connect(list_, &QListWidget::itemChanged, this, &ZebraViewWidget::detailItemChanged);
			}

			void ZebraViewWidget::showDetailTeam(const QString& key)
			{
				int row = 0;

				list_->clear();
				for (auto m : dataModel()->matches())
				{
					if (m->hasZebra())
					{
						Alliance c;
						int slot;

						if (m->teamToAllianceSlot(key, c, slot))
						{
							QString mkey = m->key();
							QListWidgetItem* item = new QListWidgetItem(m->title());
							item->setData(Qt::UserRole, mkey);
							Qt::ItemFlags f = Qt::ItemFlag::ItemIsUserCheckable | Qt::ItemFlag::ItemIsEnabled;
							item->setFlags(f);

							if (keys_.contains(mkey))
								item->setCheckState(Qt::CheckState::Checked);
							else
								item->setCheckState(Qt::CheckState::Unchecked);

							list_->addItem(item);
						}
					}
				}

				connect(list_, &QListWidget::itemChanged, this, &ZebraViewWidget::detailItemChanged);
			}

			void ZebraViewWidget::detailItemChanged(QListWidgetItem* item)
			{
				QVariant v = box_->itemData(box_->currentIndex());

				QString key = item->data(Qt::UserRole).toString();
				if (item->checkState() == Qt::CheckState::Checked && !keys_.contains(key))
				{
					keys_.push_back(key);
					if (matches_->isChecked())
						createPlotMatchWithKeys(v.toString());
					else
						createPlotTeamWithKeys(v.toString());
				}
				else if (item->checkState() == Qt::CheckState::Unchecked && keys_.contains(key))
				{
					keys_.removeOne(key);
					if (matches_->isChecked())
						createPlotMatchWithKeys(v.toString());
					else
						createPlotTeamWithKeys(v.toString());
				}
			}

			void ZebraViewWidget::rangeChanged(double minv, double maxv)
			{
				for (auto t : field_->tracks())
				{
					t->setRange(slider_->rangeStart(), slider_->rangeEnd());
				}

				field_->update();
			}

			void ZebraViewWidget::clearView()
			{
				box_->clear();
				field_->clearTracks();
			}

			void ZebraViewWidget::refreshView()
			{
				matches_->setChecked(true);
				matchesSelected(true);
			}

			void ZebraViewWidget::matchesSelected(bool checked)
			{
				if (!checked)
					return;

				setNeedRefresh();
				box_->clear();
				if (dataModel() != nullptr)
				{
					for (auto m : dataModel()->matches())
					{
						if (!m->zebra().isEmpty())
							box_->addItem(m->title(), m->key());
					}
					box_->setCurrentIndex(0);
					createPlot();
				}
			}

			void ZebraViewWidget::robotSelected(bool checked)
			{
				if (!checked)
					return;

				setNeedRefresh();
				box_->clear();
				if (dataModel() != nullptr)
				{
					auto dm = dataModel();
					if (dm != nullptr) {
						std::list<std::shared_ptr<const DataModelTeam>> teams = dm->teams();
						teams.sort([](std::shared_ptr<const DataModelTeam> a, std::shared_ptr<const DataModelTeam> b) -> bool
							{
								return a->number() < b->number();
							});

						for (auto t : teams)
							box_->addItem(QString::number(t->number()) + " - " + t->nick(), t->key());
					}
					box_->setCurrentIndex(0);
					createPlot();
				}
			}

			void ZebraViewWidget::comboxChanged(int which)
			{
				setNeedRefresh();
				createPlot();
				field_->update();
			}

			void ZebraViewWidget::createPlot()
			{
				QVariant v = box_->itemData(box_->currentIndex());
				
				if (v.type() == QVariant::String)
				{
					if (matches_->isChecked())
					{
						createPlotMatch(v.toString());
						showDetailMatch(v.toString());
					}
					else
					{
						createPlotTeam(v.toString());
						showDetailTeam(v.toString());
					}
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

			void ZebraViewWidget::getTimes(const QJsonArray& array, double& minv, double& maxv)
			{
				minv = std::numeric_limits<double>::max();
				maxv = std::numeric_limits<double>::min();

				for (int i = 0; i < array.size(); i++)
				{
					if (array[i].isDouble())
					{
						double d = array[i].toDouble();
						if (d > maxv)
							maxv = d;
						if (d < minv)
							minv = d;
					}
				}
			}

			void ZebraViewWidget::createPlotMatch(const QString& key)
			{
				if (!needsRefresh())
					return;

				keys_.clear();

				auto m = dataModel()->findMatchByKey(key);
				if (m == nullptr)
					return;

				Alliance c = Alliance::Red;
				for (int i = 1; i <= 3; i++)
				{
					keys_.push_back(m->team(c, i));
				}
				c = Alliance::Blue;
				for (int i = 1; i <= 3; i++)
				{
					keys_.push_back(m->team(c, i));
				}

				createPlotMatchWithKeys(key);
			}

			void ZebraViewWidget::createPlotMatchWithKeys(const QString &key)
			{
				field_->clearTracks();

				auto m = dataModel()->findMatchByKey(key);
				if (m == nullptr)
					return;

				const QJsonObject& zebra = m->zebra();
				QStringList keys = zebra.keys();

				if (!zebra.contains("times") || !zebra.value("times").isArray())
					return;

				if (!zebra.contains("alliances") || !zebra.value("alliances").isObject())
					return;

				double minv, maxv;
				getTimes(zebra.value("times").toArray(), minv, maxv);
				slider_->setMinimum(minv);
				slider_->setMaximum(maxv);
				slider_->setRangeStart(minv);
				slider_->setRangeEnd(maxv);

				std::vector<std::shared_ptr<RobotTrack>> tracks;
				Alliance c = Alliance::Red;
				for (int i = 1; i <= 3; i++)
				{
					QString tmkey = m->team(c, i);
					if (keys_.contains(tmkey))
					{
						auto t = std::make_shared<RobotTrack>(tmkey, matchRobotColor(c, i), m->title());
						t->setRange(slider_->rangeStart(), slider_->rangeEnd());
						tracks.push_back(t);
					}
					else
					{
						tracks.push_back(nullptr);
					}
				}
				c = Alliance::Blue;
				for (int i = 1; i <= 3; i++)
				{
					QString tmkey = m->team(c, i);
					if (keys_.contains(tmkey))
					{
						auto t = std::make_shared<RobotTrack>(tmkey, matchRobotColor(c, i), m->title());
						t->setRange(slider_->rangeStart(), slider_->rangeEnd());
						tracks.push_back(t);
					}
					else
					{
						tracks.push_back(nullptr);
					}
				}

				QJsonArray times = zebra.value("times").toArray();
				for (int i = 0; i < times.size(); i++) {
					if (!times[i].isDouble())
						return;

					for (int j = 0; j < tracks.size(); j++)
					{
						if (tracks[j] != nullptr)
							tracks[j]->addTime(times[i].toDouble());
					}
				}

				const QJsonObject& aobj = zebra.value("alliances").toObject();

				if (!aobj.contains("blue") || !aobj.value("blue").isArray())
					return;

				if (!aobj.contains("red") || !aobj.value("red").isArray())
					return;

				processAlliance(aobj.value("red").toArray(), Alliance::Red, tracks);
				processAlliance(aobj.value("blue").toArray(), Alliance::Blue, tracks);

				for (auto t : tracks)
				{
					if (t != nullptr && t->locSize() > 1)
						field_->addTrack(t);
				}
				field_->update();
			}

			bool ZebraViewWidget::extractOneAlliance(const QJsonArray& arr, Alliance c, int slot, std::shared_ptr<RobotTrack> track)
			{
				int which = slot - 1;
				if (!arr[which].isObject())
					return false;

				const QJsonObject& obj = arr[which].toObject();

				if (!obj.contains("xs") || !obj.value("xs").isArray())
					return false;

				if (!obj.contains("ys") || !obj.value("ys").isArray())
					return false;

				const QJsonArray& xa = obj.value("xs").toArray();
				const QJsonArray& ya = obj.value("ys").toArray();

				if (xa.size() != ya.size())
					return false;

				for (int k = 0; k < xa.size(); k++)
				{
					if (xa[k].isUndefined() || ya[k].isUndefined())
						continue;

					if (xa[k].isNull() || ya[k].isNull())
						continue;

					if (!xa[k].isDouble() || !ya[k].isDouble())
						continue;

					double xv = xa[k].toDouble();
					double yv = ya[k].toDouble();

					track->addPoint(xero::paths::Translation2d(xv, yv));
				}
				return true;
			}

			void ZebraViewWidget::processAlliance(const QJsonArray& arr, Alliance c, std::vector<std::shared_ptr<RobotTrack>>& tracks)
			{
				for (int slot = 1; slot <= 3; slot++)
				{
					int which = slot - 1;
					if (c == Alliance::Blue)
						which += 3;

					if (tracks[which] != nullptr)
						extractOneAlliance(arr, c, slot, tracks[which]);
				}
			}

			void ZebraViewWidget::createPlotTeam(const QString& key)
			{
				if (!needsRefresh())
					return;

				keys_.clear();
				for (auto m : dataModel()->matches())
				{
					Alliance c;
					int slot;

					if (!m->teamToAllianceSlot(key, c, slot))
						continue;

					keys_.push_back(m->key());
				}

				createPlotTeamWithKeys(key);
			}

			void ZebraViewWidget::createPlotTeamWithKeys(const QString &key)
			{
				std::list<std::shared_ptr<RobotTrack>> tracks;

				for (auto mkey: keys_)
				{
					auto m = dataModel()->findMatchByKey(mkey);

					Alliance c;
					int slot;

					if (!m->teamToAllianceSlot(key, c, slot))
						continue;

					auto track = std::make_shared<RobotTrack>(key, matchRobotColor(c, 1), m->title());
					track->setRange(slider_->rangeStart(), slider_->rangeEnd());
					tracks.push_back(track);

					const QJsonObject& zebra = m->zebra();

					QJsonArray times = zebra.value("times").toArray();
					for (int i = 0; i < times.size(); i++) {
						if (!times[i].isDouble())
							return;

						track->addTime(times[i].toDouble());
					}

					if (!zebra.contains("times") || !zebra.value("times").isArray())
						continue;

					if (!zebra.contains("alliances") || !zebra.value("alliances").isObject())
						continue;

					const QJsonObject& aobj = zebra.value("alliances").toObject();

					if (!aobj.contains("blue") || !aobj.value("blue").isArray())
						return;

					if (!aobj.contains("red") || !aobj.value("red").isArray())
						return;

					const QJsonArray& arr = aobj.value(toString(c)).toArray();
					extractOneAlliance(arr, c, slot, track);
				}

				field_->clearTracks();
				for (auto t : tracks)
				{
					if (t->locSize() > 1)
						field_->addTrack(t);
				}

				field_->update();
			}
		}
	}
}

