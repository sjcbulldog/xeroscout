//
// Copyright 2020 by Jack W. (Butch) Griffin
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

#include "MatchViewWidget.h"
#include <vector>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			QStringList MatchViewWidget::headers_all_ =
			{
				"Type",
				"Set",
				"Num",
				"RTeam1",
				"RTablet1",
				"RData1",
				"RTeam2",
				"RTablet2",
				"RData2",
				"RTeam3",
				"RTablet3",
				"RData3",
				"BTeam1",
				"BTablet1",
				"BData1",
				"BTeam2",
				"BTablet2",
				"BData2",
				"BTeam3",
				"BTablet3",
				"BData3",
				""
			};

			QStringList MatchViewWidget::headers_one_ =
			{
				"Type",
				"Set",
				"Num",
				"Color",
				"Status",
				"Number",
				"Name"
			};

			MatchViewWidget::MatchViewWidget(const QString &tablet, QWidget* parent) : QTreeWidget(parent), ViewBase("MatchViewWidget")
			{
				tablet_ = tablet;

				if (tablet_.length() == 0)
				{
					setColumnCount(headers_all_.size());
					setHeaderLabels(headers_all_);
				}
				else
				{
					setColumnCount(headers_one_.size());
					setHeaderLabels(headers_one_);
				}

				if (tablet_.length() > 0) {
					QFont f = font();
					f.setPointSizeF(18.0);
					setFont(f);
				}
			}

			MatchViewWidget::~MatchViewWidget()
			{
			}

			QString MatchViewWidget::genStatusText(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> match, xero::scouting::datamodel::Alliance a, int slot, bool tt)
			{
				QString ret;


				if (tt)
				{
					if (match->hasScoutingData(a, slot))
						ret = "Has Scouting Data";
					else
						ret = "No Scouting Data";
				}
				else
				{
					if (match->hasScoutingData(a, slot))
						ret = "Y";
					else
						ret = "N";
				}

				if (tt)
				{
					if (match->hasBlueAllianceData())
						ret += "\nHas Blue Alliance Match Data";
					else
						ret += "\nNo Blue Alliance Match Data";
				}
				else
				{
					if (match->hasBlueAllianceData())
						ret += "-B";
				}

				if (tt)
				{
					if (match->hasZebra())
						ret += "\nHas Zebra Data";
					else
						ret += "\nNo Zebra Data";
				}
				else
				{
					if (match->hasZebra())
						ret += "-Z";
				}

				return ret;
			}

			void MatchViewWidget::setScoutingField(const QString& key, Alliance c, int slot)
			{
				int alloff = (c == Alliance::Red ? 5 : 14);
				int col;

				auto m = dataModel()->findMatchByKey(key);
				QString txt = genStatusText(m, c, slot, false);
				QString tt = genStatusText(m, c, slot, true);

				if (tablet_.length() == 0)
				{
					col = alloff + 3 * (slot - 1);
				}
				else
				{
					col = 4;
				}

				for (int i = 0; i < topLevelItemCount(); i++)
				{
					QTreeWidgetItem* item = topLevelItem(i);
					QString itemkey = item->data(0, Qt::UserRole).toString();

					if (itemkey == key)
					{
						item->setTextAlignment(col, Qt::AlignHCenter);
						item->setToolTip(col, tt);

						if (txt.at(0) == "Y")
						{
							item->setText(col, txt);
							item->setBackgroundColor(col, QColor(0, 255, 0, 255));
						}
						else
						{
							item->setText(col, txt);
							item->setBackgroundColor(col, QColor(255, 0, 0, 255));
						}
					}
				}
			}

			int MatchViewWidget::putAllianceData(std::shared_ptr<const DataModelMatch> m, QTreeWidgetItem* i, Alliance a, int index)
			{
				for (int slot = 1; slot <= 3; slot++) {
					QString txt = genStatusText(m, a, slot, false);
					QString tt = genStatusText(m, a, slot, true);
					QString key = m->team(a, slot);

					auto t = dataModel()->findTeamByKey(key);

					QString tmtt = QString::number(t->number()) + " - " + t->name();
					i->setText(index, key);
					i->setToolTip(index++, tmtt);

					const QString& tablet = m->tablet(a, slot);
					i->setText(index++, tablet);

					if (tablet_.length() == 0 || tablet_ == tablet)
					{
						i->setTextAlignment(index, Qt::AlignHCenter);
						i->setToolTip(index, tt);
						if (m->hasScoutingData(a, slot))
						{
							i->setText(index, txt);
							i->setBackgroundColor(index, QColor(0, 255, 0, 255));
						}
						else
						{
							i->setText(index, txt);
							i->setBackgroundColor(index, QColor(255, 0, 0, 255));
						}
					}
					else
					{
						i->setText(index, "");
					}
					index++;
				}

				return index;
			}

			void MatchViewWidget::putOneOne(std::shared_ptr<const DataModelMatch> m)
			{
				Alliance color;
				int slot;

				if (!m->tabletToAllianceSlot(tablet_, color, slot))
					return;

				int index = 0;
				QTreeWidgetItem* i = new QTreeWidgetItem(this);

				i->setText(index++, m->compType());
				i->setData(0, Qt::UserRole, QVariant(m->key()));

				if (m->compType() != "qm")
					i->setText(index, QString::number(m->set()));
				else
					i->setText(index, "-");
				index++;

				i->setText(index++, QString::number(m->match()));
				i->setText(index++, toString(color));

				i->setTextAlignment(index, Qt::AlignHCenter);
				if (m->hasScoutingData(color, slot)) 
				{
					i->setText(index, "Y");
					i->setBackgroundColor(index, QColor(0, 255, 0, 255));
				}
				else
				{
					i->setText(index, "N");
					i->setBackgroundColor(index, QColor(255, 0, 0, 255));
				}
				index++;

				QString tkey = m->team(color, slot);
				auto team = dataModel()->findTeamByKey(tkey);

				i->setText(index++, QString::number(team->number()));
				i->setText(index++, team->name());

				addTopLevelItem(i);
			}

			void MatchViewWidget::putOneAll(std::shared_ptr<const DataModelMatch> m)
			{
				int index = 0;
				QTreeWidgetItem* i = new QTreeWidgetItem(this);
				i->setText(index++, m->compType());
				i->setData(0, Qt::UserRole, QVariant(m->key()));

				if (m->compType() != "qm")
					i->setText(index, QString::number(m->set()));
				else
					i->setText(index, "-");
				index++;

				i->setText(index++, QString::number(m->match()));

				index = putAllianceData(m, i, Alliance::Red, index);
				index = putAllianceData(m, i, Alliance::Blue, index);

				addTopLevelItem(i);
			}

			int MatchViewWidget::matchClass(const QString& comp)
			{
				int ret = -1;

				if (comp == "qm")
					ret = 0;
				else if (comp == "qf")
					ret = 1;
				else if (comp == "sf")
					ret = 2;
				else if (comp == "ef")
					ret = 3;
				else
					ret = 4;

				return ret;
			}

			bool MatchViewWidget::wayToSort(std::shared_ptr<const DataModelMatch> one, std::shared_ptr<const DataModelMatch> two)
			{
				bool ret = false;

				int cl1 = matchClass(one->compType());
				int cl2 = matchClass(two->compType());

				if (cl1 != cl2)
					ret = (cl1 < cl2);
				else if (cl1 > 0)
				{
					if (one->set() == two->set())
						ret = one->match() < two->match();
					else
						ret = one->set() < two->set();
				}
				else
					ret = one->match() < two->match();

				return ret;
			}

			void MatchViewWidget::setData(std::list<std::shared_ptr<const DataModelMatch>> matches)
			{
				matches_.clear();

				matches_.insert(matches_.begin(), matches.begin(), matches.end());
				matches_.sort(&MatchViewWidget::wayToSort);

				matches_ = matches;
				refreshView();
			}

			void MatchViewWidget::refreshView()
			{
				setUpdatesEnabled(false);
				clear();

				for (auto m : matches_)
				{
					if (tablet_.size() == 0)
						putOneAll(m);
					else
						putOneOne(m);
				}

				for (int i = 0; i < columnCount(); i++)
					resizeColumnToContents(i);

				setUpdatesEnabled(true);
			}

			void MatchViewWidget::clearView()
			{
				while (topLevelItemCount() > 0) {
					auto item = takeTopLevelItem(0);
					delete item;
				}
			}
		}
	}
}
