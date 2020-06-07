//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
// 

#include "ZebraViewWidget.h"
#include "RobotTrack.h"
#include <QBoxLayout>

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

				field_ = new PathFieldView(this);
				QSizePolicy p(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
				field_->setSizePolicy(p);
				vlay->addWidget(field_);

				slider_ = new TimeBoundWidget(0.0, 135.0, this);
				vlay->addWidget(slider_);
				connect(slider_, &TimeBoundWidget::rangeChanged, this, &ZebraViewWidget::rangeChanged);

				matches_->setChecked(true);
				matchesSelected(true);
			}

			ZebraViewWidget::~ZebraViewWidget()
			{
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
							box_->addItem(QString::number(t->number()) + " - " + t->name(), t->key());
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
				if (matches_->isChecked())
				{
					QVariant v = box_->itemData(box_->currentIndex());
					if (v.type() == QVariant::String)
						createPlotMatch(v.toString());
				}
				else
				{
					QVariant v = box_->itemData(box_->currentIndex());
					if (v.type() == QVariant::String)
						createPlotTeam(v.toString());
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

			void ZebraViewWidget::createPlotMatch(const QString &key)
			{
				if (!needsRefresh())
					return;

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
					auto t = std::make_shared<RobotTrack>(m->team(c, i), matchRobotColor(c, i), m->title());
					t->setRange(slider_->rangeStart(), slider_->rangeEnd());
					tracks.push_back(t);
				}
				c = Alliance::Blue;
				for (int i = 1; i <= 3; i++)
				{
					auto t = std::make_shared<RobotTrack>(m->team(c, i), matchRobotColor(c, i), m->title());
					t->setRange(slider_->rangeStart(), slider_->rangeEnd());
					tracks.push_back(t);
				}

				QJsonArray times = zebra.value("times").toArray();
				for (int i = 0; i < times.size(); i++) {
					if (!times[i].isDouble())
						return;

					for (int j = 0; j < tracks.size(); j++)
						tracks[j]->addTime(times[i].toDouble());
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
					if (t->locSize() > 1)
						field_->addTrack(t);
				}
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

					extractOneAlliance(arr, c, slot, tracks[which]);
				}
			}

			void ZebraViewWidget::createPlotTeam(const QString& key)
			{
				if (!needsRefresh())
					return;

				std::list<std::shared_ptr<RobotTrack>> tracks;

				for (auto m : dataModel()->matches())
				{
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
			}
		}
	}
}

