#include "FormItemDisplay.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			FormItemDisplay::FormItemDisplay(const FormItemDesc *desc, QWidget* parent) : QWidget(parent)
			{
				desc_ = desc;
			}

			FormItemDisplay::~FormItemDisplay()
			{
			}
		}
	}
}
