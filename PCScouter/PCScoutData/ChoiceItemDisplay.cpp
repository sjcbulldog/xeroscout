#include "ChoiceItemDisplay.h"
#include "ChoiceFormItem.h"
#include <QBoxLayout>
#include <QLabel>
#include <QComboBox>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			ChoiceItemDisplay::ChoiceItemDisplay(const FormItemDesc *desc, QWidget* parent) : FormItemDisplay(desc, parent)
			{
				QWidget* w = new QWidget();
				QHBoxLayout* layout = new QHBoxLayout();
				w->setLayout(layout);

				QLabel* label = new QLabel(desc->display(), w);
				layout->addWidget(label);
				QFont font = label->font();
				font.setPointSizeF(16.0);
				label->setFont(font);

				const ChoiceFormItem* cdesc = dynamic_cast<const ChoiceFormItem*>(desc);
				box_ = new QComboBox(parent);
				layout->addWidget(box_);
				for (const QString& choice : cdesc->choices())
					box_->addItem(choice);

				box_->setCurrentIndex(0);

				font = box_->font();
				font.setPointSizeF(16.0);
				box_->setFont(font);
			}

			ChoiceItemDisplay::~ChoiceItemDisplay()
			{
			}

			void ChoiceItemDisplay::setValues(const DataCollection& data)
			{
				assert(data.count() == 1);
				assert(data.data(0).type() == QVariant::Type::String);

				for (int i = 0; i < box_->count(); i++)
				{
					if (box_->itemText(i) == data.data(0).toString())
					{
						box_->setCurrentIndex(i);
						break;
					}
				}
			}

			DataCollection ChoiceItemDisplay::getValues()
			{
				DataCollection d;
				d.add(desc()->tag(), QVariant(box_->currentText()));
				return d;
			}
		}
	}
}
