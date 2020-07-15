#include "PickListEditor.h"
#include "PickListEditorWidget.h"
#include <QBoxLayout>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			PickListEditor::PickListEditor(QWidget* parent, const QString& name) : ViewBase(name), QWidget(parent)
			{
				QVBoxLayout* lay = new QVBoxLayout();
				setLayout(lay);

				area_ = new QScrollArea(this);
				editor_ = new PickListEditorWidget(area_);
				area_->setWidget(editor_);
				lay->addWidget(area_);

				area_->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);
				area_->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);
				(void)connect(editor_, &PickListEditorWidget::picklistChanged, this, &PickListEditor::picklistDataChanged);
			}

			PickListEditor::~PickListEditor()
			{
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
					editor_->setPickList(dataModel()->picklist());
			}
		}
	}
}
