#pragma once
#include "FormItemDisplay.h"
#include <QLineEdit>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class TextItemDisplay : public FormItemDisplay
			{
			public:
				TextItemDisplay(ImageSupplier& images, const FormItemDesc *desc, QWidget* parent);
				virtual ~TextItemDisplay();

				virtual void setValues(const DataCollection& data);
				virtual DataCollection getValues();

			private:
				QLineEdit* edit_;
			};
		}
	}
}

