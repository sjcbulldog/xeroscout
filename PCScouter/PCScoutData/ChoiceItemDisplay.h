#pragma once
#include "FormItemDisplay.h"
#include <QStringList>
#include <QComboBox>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ChoiceItemDisplay : public FormItemDisplay
			{
			public:
				ChoiceItemDisplay(const FormItemDesc *desc, QWidget *parent);
				virtual ~ChoiceItemDisplay();

				virtual void setValues(const DataCollection& data);
				virtual DataCollection getValues();

			private:
				QComboBox* box_;
			};
		}
	}
}
