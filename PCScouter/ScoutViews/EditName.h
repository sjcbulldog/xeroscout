#pragma once

#include <QLineEdit>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class EditName : public QLineEdit
			{
				Q_OBJECT

			public:
				EditName(QWidget* parent = Q_NULLPTR);
				~EditName();

			signals:
				void escapePressed();

			protected:
				virtual void keyPressEvent(QKeyEvent* ev) override;

			private:
				void emitEscapePressed();

			private:
			};
		}
	}
}

