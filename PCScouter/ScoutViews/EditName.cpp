#include "EditName.h"
#include <QKeyEvent>

namespace xero
{
	namespace scouting
	{
		namespace views
		{

			EditName::EditName(QWidget* parent) : QLineEdit(parent)
			{
			}

			EditName::~EditName()
			{
			}

			void EditName::keyPressEvent(QKeyEvent* ev)
			{
				if (ev->key() == Qt::Key_Escape)
				{
					emitEscapePressed();
					return;
				}

				QLineEdit::keyPressEvent(ev);
			}

			void EditName::emitEscapePressed()
			{
				emit escapePressed();
			}
		}
	}
}
