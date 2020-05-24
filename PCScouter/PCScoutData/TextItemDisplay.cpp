#include "TextItemDisplay.h"
#include "TextFormItem.h"
#include <QBoxLayout>
#include <QLabel>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			TextItemDisplay::TextItemDisplay(const FormItemDesc* desc, QWidget* parent) : FormItemDisplay(desc, parent)
			{
				QWidget* w = new QWidget();
				QHBoxLayout* layout = new QHBoxLayout();
				w->setLayout(layout);

				QLabel* label = new QLabel(desc->display(), w);
				layout->addWidget(label);
				QFont font = label->font();
				font.setPointSizeF(16.0);
				label->setFont(font);

				auto tdesc = dynamic_cast<const TextFormItem *>(desc);

				QLineEdit* edit = new QLineEdit(parent);
				layout->addWidget(edit);
				font = edit->font();
				font.setPointSizeF(16.0);
				edit->setFont(font);
				edit->setMaxLength(tdesc->maxLen());
			}

			TextItemDisplay::~TextItemDisplay()
			{
			}

			void TextItemDisplay::setValues(const DataCollection& data)
			{
				assert(data.count() == 1);
				assert(data.data(0).type() == QVariant::Type::String);

				edit_->setText(data.data(0).toString());
			}

			DataCollection TextItemDisplay::getValues()
			{
				DataCollection d;
				d.add(desc()->tag(), QVariant(edit_->text()));

				return d;
			}
		}
	}
}
