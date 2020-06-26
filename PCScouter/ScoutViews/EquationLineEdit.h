#pragma once
#include <QLineEdit>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class EquationLineEdit : public QLineEdit
			{
			public:
				EquationLineEdit(QWidget* parent);
				virtual ~EquationLineEdit();

				void setWordList(const QStringList& list);

			protected:
				void keyPressEvent(QKeyEvent* ev);

			private:
				void insertCompletion(QString arg);
				QString cursorWord(const QString& sentence) const;

			protected:
				QCompleter* completer_;
			};
		}
	}
}

