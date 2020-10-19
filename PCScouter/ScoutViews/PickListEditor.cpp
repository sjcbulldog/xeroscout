#include "PickListEditor.h"
#include "PickListEditorWidget.h"
#include <QBoxLayout>
#include <QScrollBar>
#include <algorithm>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			PickListEditor::PickListEditor(QWidget* parent, const QString& name) : ViewBase(name), QWidget(parent)
			{
				QHBoxLayout* lay = new QHBoxLayout();
				setLayout(lay);

				teamlist_area_ = new QScrollArea(this);
				teams_ = new TeamSelectedWidget(teamlist_area_);
				teams_->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
				(void)connect(teams_, &TeamSelectedWidget::teamStateChanged, this, &PickListEditor::teamChanged);

				teamlist_area_->setWidget(teams_);
				teamlist_area_->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred);
				teamlist_area_->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
				teamlist_area_->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

				int sbwidth = teamlist_area_->verticalScrollBar()->sizeHint().width();
				int width = teams_->width() + sbwidth;
				teamlist_area_->setMinimumWidth(width);
				lay->addWidget(teamlist_area_);

				picklist_area_ = new QScrollArea(this);
				editor_ = new PickListEditorWidget(picklist_area_);
				picklist_area_->setWidget(editor_);
				lay->addWidget(picklist_area_);

				picklist_area_->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
				picklist_area_->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
				(void)connect(editor_, &PickListEditorWidget::picklistChanged, this, &PickListEditor::picklistDataChanged);
			}

			PickListEditor::~PickListEditor()
			{
			}

			void PickListEditor::teamChanged(int team, bool selected)
			{
				editor_->teamSelected(team, selected);
			}

			void PickListEditor::picklistDataChanged()
			{
				dataModel()->replacePickList(editor_->picklist());
			}

			void PickListEditor::clearView()
			{
				editor_->clearPickList();
			}

			void PickListEditor::refreshView()
			{
				if (dataModel() != nullptr)
				{
					editor_->setPickList(dataModel()->picklist());

					std::vector<int> teams;
					for (auto entry : dataModel()->picklist())
					{
						teams.push_back(entry.team());
					}

					std::sort(teams.begin(), teams.end(), [](const int a, const int b)->bool { return a > b; });
					teams_->setTeams(teams);
				}
			}
		}
	}
}
