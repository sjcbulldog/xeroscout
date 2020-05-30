#include "DataMergeListWidget.h"
#include <QBoxLayout>

using namespace xero::scouting::datamodel;

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			DataMergeListWidget::DataMergeListWidget(QWidget* parent) : QWidget(parent)
			{
				QStringList headers = { "Type", "Key", "Item" };

				QVBoxLayout* lay = new QVBoxLayout();
				setLayout(lay);

				tree_ = new QTreeWidget(this);
				lay->addWidget(tree_);

				tree_->setHeaderLabels(headers);
				tree_->setColumnCount(3);

				(void)connect(tree_, &QTreeWidget::itemDoubleClicked, this, &DataMergeListWidget::doubleClicked);
			}

			DataMergeListWidget::~DataMergeListWidget()
			{
			}

			void DataMergeListWidget::clearView()
			{
				while (tree_->topLevelItemCount() != 0)
				{
					QTreeWidgetItem* item = tree_->takeTopLevelItem(0);
					delete item;
				}
			}

			void DataMergeListWidget::refreshView()
			{
				auto dm = dataModel();

				for (auto p : dm->teams())
				{
					if (p->teamScoutingDataList().size() > 1)
					{
						QTreeWidgetItem* item = new QTreeWidgetItem(tree_);
						item->setText(0, "Pit Scout");
						item->setText(1, p->key());
						item->setText(2, p->name());

						tree_->addTopLevelItem(item);
					}
				}

				for (auto m : dm->matches())
				{
					Alliance c = Alliance::Red;
					for (int i = 1; i <= 3; i++)
					{
						if (m->scoutingDataList(c, i).size() > 1)
						{
							QString title = m->title();
							QString team = m->team(c, i);
							auto t = dm->findTeamByKey(team);
							title += " - " + t->name();

							QTreeWidgetItem* item = new QTreeWidgetItem(tree_);
							item->setText(0, "Match Scout");
							item->setText(1, m->key());
							item->setText(2, title);

							item->setData(0, Qt::UserRole, QVariant(static_cast<int>(c)));
							item->setData(1, Qt::UserRole, QVariant(i));

							tree_->addTopLevelItem(item);
						}
					}

					c = Alliance::Blue;
					for (int i = 1; i <= 3; i++)
					{
						if (m->scoutingDataList(c, i).size() > 1)
						{
							QString title = m->title();
							QString team = m->team(c, i);
							auto t = dm->findTeamByKey(team);
							title += " - " + t->name();

							QTreeWidgetItem* item = new QTreeWidgetItem(tree_);
							item->setText(0, "Match Scout");
							item->setText(1, m->key());
							item->setText(2, title);

							item->setData(0, Qt::UserRole, QVariant(static_cast<int>(c)));
							item->setData(1, Qt::UserRole, QVariant(i));

							tree_->addTopLevelItem(item);
						}
					}
				}

				tree_->resizeColumnToContents(0);
				tree_->resizeColumnToContents(1);
				tree_->resizeColumnToContents(2);
			}

			void DataMergeListWidget::doubleClicked(QTreeWidgetItem* item, int col)
			{
				auto dm = dataModel();
				QString key = item->text(1);

				if (dm->findTeamByKey(key) != nullptr)
				{
					emit mergePit(key);
				}
				else if (dm->findMatchByKey(key) != nullptr)
				{
					Alliance c = static_cast<Alliance>(item->data(0, Qt::UserRole).toInt());
					int slot = item->data(1, Qt::UserRole).toInt();
					emit mergeMatch(key, c, slot);
				}
			}
		}
	}
}
