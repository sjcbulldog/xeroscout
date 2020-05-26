#pragma once

#include "ImageSupplier.h"
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
				FormItemDisplay(ImageSupplier &images, const FormItemDesc *desc, QWidget* parent);
				virtual ~FormItemDisplay();

				virtual void setValues(const DataCollection& data) = 0;
				virtual DataCollection getValues() = 0;

				const FormItemDesc *desc() {
					return desc_;
				}

			protected:
				ImageSupplier& images() {
					return images_;
				}

			private:
				const FormItemDesc * desc_;
				ImageSupplier& images_;
			};
		}
	}
}
