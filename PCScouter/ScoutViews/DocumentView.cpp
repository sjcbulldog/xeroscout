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

#include "DocumentView.h"
#include "FormView.h"
#include "TeamScheduleViewWidget.h"
#include "MatchViewWidget.h"
#include "DataSetViewWidget.h"
#include "QueryViewWidget.h"
#include "ChangeHistoryView.h"
#include "DataMergeListWidget.h"
#include "ZebraViewWidget.h"
#include "PreMatchGraphView.h"
#include "AllianceGraphView.h"
#include "TeamSummaryWidget.h"
#include <QLabel>
#include <QDebug>
#include <QFont>
#include <QCoreApplication>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			DocumentView::DocumentView(ImageManager &mgr, int year, const QString &tablet, QWidget* parent): images_(mgr)
			{
				bool gamemgr = true;
				QString yearstr = QString::number(year);

				QString exedir = QCoreApplication::applicationDirPath();

				field_mgr_.addDefaultDirectory((exedir + "/fields").toStdString());
				field_mgr_.copyDefaults("fields");
				if (!field_mgr_.initialize())
					gamemgr = false;

				QLabel* label = new QLabel("No Event Loaded", this);
				label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
				QFont font = label->font();
				font.setPointSizeF(18.0);
				label->setFont(font);

				addWidget(label);																				// 0
				addWidget(new FormView(images_, "TeamTemplate", "Preview", QColor(0, 255, 0, 255), this));		// 1
				addWidget(new FormView(images_, "MatchTemplate", "Preview", QColor(255, 0, 0, 255), this));		// 2
				addWidget(new FormView(images_, "MatchTemplate", "Preview", QColor(0, 0, 255, 255), this));		// 3
				addWidget(new TeamScheduleViewWidget(tablet, this));											// 4
				addWidget(new MatchViewWidget(tablet, this));													// 5
				addWidget(new DataSetViewWidget("matchdata", true, this));										// 6
				addWidget(new DataSetViewWidget("teamdata", true, this));			  							// 7
				addWidget(new QueryViewWidget(this));															// 8
				addWidget(new TeamSummaryWidget(this));															// 9
				addWidget(new DataSetViewWidget("allteam", false, this));										// 10
				addWidget(new ChangeHistoryView(this));															// 11
				addWidget(new DataMergeListWidget(this));														// 12

				ZebraViewWidget* zview = new ZebraViewWidget(this);
				addWidget(zview);																				// 13

				GraphView* gview = new PreMatchGraphView(this);
				gview->create();
				addWidget(gview);																				// 14

				gview = new AllianceGraphView(this);
				gview->create();
				addWidget(gview);																				// 15

				if (gamemgr)
				{
					std::shared_ptr<GameField> fld = nullptr;

					for (auto f : field_mgr_.getFields()) {
						QString name = f->getName().c_str();
						if (name.contains(yearstr))
						{
							fld = f;
							break;
						}
					}

					if (fld != nullptr)
						zview->field()->setField(fld);
				}

				setCurrentIndex(0);
				view_ = ViewType::NoModelView;

				QTreeWidget* tree;

				tree = dynamic_cast<QTreeWidget*>(getWidget(ViewType::TeamView));
				(void)connect(tree, &QTreeWidget::itemDoubleClicked, this, &DocumentView::teamItemDoubleClicked);

				tree = dynamic_cast<QTreeWidget*>(getWidget(ViewType::MatchView));
				(void)connect(tree, &QTreeWidget::itemDoubleClicked, this, &DocumentView::matchItemDoubleClicked);
			}

			DocumentView::~DocumentView()
			{
			}

			void DocumentView::clearAll()
			{
				for (int i = 0; i < count(); i++)
				{
					QWidget* w = widget(i);


					ViewBase* vb = dynamic_cast<ViewBase*>(w);
					if (vb != nullptr)
						vb->clearView();
				}
			}

			void DocumentView::refreshAll()
			{
				for (int i = 0; i < count(); i++)
				{
					ViewBase* vb = dynamic_cast<ViewBase*>(widget(i));
					if (vb != nullptr)
						vb->refreshView();
				}
			}

			void DocumentView::needsRefreshAll()
			{
				for (int i = 0; i < count(); i++) {

					MatchViewWidget* vw = dynamic_cast<MatchViewWidget*>(widget(i));
					ViewBase* vb = dynamic_cast<ViewBase*>(widget(i));

					if (vb != nullptr)
						vb->setNeedRefresh();
				}

				ViewBase* vb = dynamic_cast<ViewBase*>(widget(static_cast<int>(view_)));
				if (vb != nullptr)
					vb->refreshView();
			}

			bool DocumentView::createFetchFormView(const QString& key, const QString& title, QColor c, int& index)
			{
				bool ret = false;
				index = -1;

				auto it = scouting_forms_.find(key);
				if (it == scouting_forms_.end())
				{
					QWidget* w = new FormView(images_, key, title, c, this);
					scouting_forms_.insert(std::make_pair(key, w));

					index = count();
					addWidget(w);
					ret = true;
				}
				else
				{
					QWidget* w = it->second;
					for (int i = 0; i < count(); i++) {
						if (widget(i) == w)
						{
							index = i;
							break;
						}
					}
				}

				return ret;
			}

			void DocumentView::teamItemDoubleClicked(QTreeWidgetItem* item, int col)
			{
				const QVariant& v = item->data(0, Qt::UserRole);

				if (v.isValid() && v.type() == QVariant::String)
					emit itemDoubleClicked(ViewType::TeamView, v.toString());
			}

			void DocumentView::matchItemDoubleClicked(QTreeWidgetItem* item, int col)
			{
				const QVariant& v = item->data(0, Qt::UserRole);

				if (v.isValid() && v.type() == QVariant::String)
					emit itemDoubleClicked(ViewType::MatchView, v.toString());
			}

			void DocumentView::setViewType(ViewType which)
			{
				int index = static_cast<int>(which);
				setCurrentIndex(index);
				view_ = which;

				ViewBase* vb = dynamic_cast<ViewBase*>(widget(index));
				if (vb != nullptr)
					vb->madeActive();
			}

			QWidget* DocumentView::getWidget(int index)
			{
				if (index < 0 || index >= count())
					return nullptr;

				return widget(index);
			}

			void DocumentView::deleteWidget(int index)
			{
				assert(index == count() - 1);
				QWidget* w = widget(index);

				// Remove this widget from the map
				for (auto it = scouting_forms_.begin(); it != scouting_forms_.end(); it++)
				{
					if (it->second == w)
					{
						scouting_forms_.erase(it);
						break;
					}
				}

				// Remove this widget from the stacked layout
				removeWidget(w);

				// Delete this widget
				delete w;
			}
		}
	}
}
