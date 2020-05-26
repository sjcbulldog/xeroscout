#include "NumericItemDisplay.h"
#include "NumericFormItem.h"
#include <QLineEdit>
#include <QBoxLayout>
#include <QLabel>
#include <QIntValidator>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			NumericItemDisplay::NumericItemDisplay(ImageSupplier &images, const FormItemDesc *desc, QWidget* parent) : FormItemDisplay(images, desc, parent)
			{
				QWidget* w = new QWidget();
				QHBoxLayout* layout = new QHBoxLayout();
				w->setLayout(layout);

				QLabel* label = new QLabel(desc->display(), w);
				layout->addWidget(label);
				QFont font = label->font();
				font.setPointSizeF(16.0);
				label->setFont(font);

				auto ndesc = dynamic_cast<const NumericFormItem *>(desc);

				edit_ = new QLineEdit(parent);
				edit_->setValidator(new QIntValidator(ndesc->minValue(), ndesc->maxValue(), parent));
				layout->addWidget(edit_);
				font = edit_->font();
				font.setPointSizeF(16.0);
				edit_->setFont(font);

				edit_->setText(QString::number(ndesc->minValue()));
			}

			NumericItemDisplay::~NumericItemDisplay()
			{
			}

			void NumericItemDisplay::setValues(const DataCollection& data)
			{
				assert(data.count() == 1);
				edit_->setText(QString::number(data.data(0).toInt()));
			}

			DataCollection NumericItemDisplay::getValues()
			{
				DataCollection d;

				int v = edit_->text().toInt();
				d.add(desc()->tag(), QVariant(v));

				return d;
			}
		}
	}
}
