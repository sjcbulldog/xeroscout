#pragma once
#include "FormItemDisplay.h"
#include <QLineEdit>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class NumericItemDisplay :public FormItemDisplay
			{
			public:
				NumericItemDisplay(ImageSupplier& images, const FormItemDesc *desc, QWidget* parent);
				virtual ~NumericItemDisplay();

				virtual void setValues(const DataCollection& data);
				virtual DataCollection getValues();

			private:
				QLineEdit* edit_;
			};

		}
	}
}
