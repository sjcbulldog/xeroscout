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
				else
				{
					QFont f = font();
					f.setPointSizeF(7.0);
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

						if (txt.at(0) == 'Y')
						{
							item->setText(col, txt);
							item->setBackground(col, QBrush(QColor(0, 255, 0, 255)));
						}
						else
						{
							item->setText(col, txt);
							item->setBackground(col, QBrush(QColor(255, 0, 0, 255)));
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

					QString tmtt = QString::number(t->number()) + " - " + t->nick();
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
							i->setBackground(index, QBrush(QColor(0, 255, 0, 255)));
						}
						else
						{
							i->setText(index, txt);
							i->setBackground(index, QBrush(QColor(255, 0, 0, 255)));
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
					i->setBackground(index, QBrush(QColor(0, 255, 0, 255)));
				}
				else
				{
					i->setText(index, "N");
					i->setBackground(index, QBrush(QColor(255, 0, 0, 255)));
				}
				index++;

				QString tkey = m->team(color, slot);
				auto team = dataModel()->findTeamByKey(tkey);

				i->setText(index++, QString::number(team->number()));
				i->setText(index++, team->nick());

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

				if (dataModel() == nullptr)
					return;

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
