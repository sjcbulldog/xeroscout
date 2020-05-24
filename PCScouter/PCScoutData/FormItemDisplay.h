#pragma once

#include "DataCollection.h"
#include "FormItemDesc.h"
#include <QWidget>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class FormItemDisplay : public QWidget
			{
			public:
				FormItemDisplay(const FormItemDesc *desc, QWidget* parent);
				virtual ~FormItemDisplay();

				virtual void setValues(const DataCollection& data) = 0;
				virtual DataCollection getValues() = 0;

				const FormItemDesc *desc() {
					return desc_;
				}

			private:
				const FormItemDesc * desc_;
			};
		}
	}
}
