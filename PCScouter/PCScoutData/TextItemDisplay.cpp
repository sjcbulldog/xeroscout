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
				QHBoxLayout* layout = new QHBoxLayout();
				setLayout(layout);

				QLabel* label = new QLabel(desc->display(), this);
				layout->addWidget(label);
				QFont font = label->font();
				font.setPointSizeF(16.0);
				label->setFont(font);

				auto tdesc = dynamic_cast<const TextFormItem *>(desc);

				edit_ = new QLineEdit(this);
				layout->addWidget(edit_);
				font = edit_->font();
				font.setPointSizeF(16.0);
				edit_->setFont(font);
				edit_->setMaxLength(tdesc->maxLen());
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
