#include "FormItemDisplay.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			FormItemDisplay::FormItemDisplay(ImageSupplier& images, const FormItemDesc *desc, QWidget* parent) : QWidget(parent), images_(images)
			{
				desc_ = desc;
			}

			FormItemDisplay::~FormItemDisplay()
			{
			}
		}
	}
}
