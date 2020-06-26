#include "EquationLineEdit.h"
#include <QCompleter>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QDebug>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			EquationLineEdit::EquationLineEdit(QWidget* w) : QLineEdit(w)
			{
				completer_ = nullptr;
			}

			EquationLineEdit::~EquationLineEdit()
			{
			}

			void EquationLineEdit::setWordList(const QStringList& list)
			{
				completer_ = new QCompleter(list);
				completer_->setWidget(this);
				connect(completer_, static_cast<void (QCompleter::*)(const QString&)>(&QCompleter::activated), this, &EquationLineEdit::insertCompletion);
			}

			void EquationLineEdit::insertCompletion(QString arg)
			{
				QString txt = text();

				txt = txt.mid(0, txt.length() - completer_->completionPrefix().length()) + arg;
				setText(txt);
				setCursorPosition(txt.length());
			}

			// Get the word under the cursor
			QString EquationLineEdit::cursorWord(const QString& sentence) const
			{
				int pos = cursorPosition();
				QString result;

				if (sentence.length() == 0)
					return result;

				if (pos == sentence.length())
					pos--;

				while (pos > 0 && (sentence[pos].isLetterOrNumber() || sentence[pos] == '_'))
					pos--;

				if(pos != 0)
					pos++;

				while (pos < sentence.length() && (sentence[pos].isLetterOrNumber() || sentence[pos] == '_'))
				{
					result += sentence[pos++];
				}

				qDebug() << "cursorWord: " << result;
				return result;
			}

			void EquationLineEdit::keyPressEvent(QKeyEvent* ev)
			{
				QLineEdit::keyPressEvent(ev);

				if (completer_ == nullptr)
					return;

				completer_->setCompletionPrefix(cursorWord(this->text()));
				if (completer_->completionPrefix().length() < 1)
				{
					completer_->popup()->hide();
					return;
				}
				QRect cr = cursorRect();
				cr.setWidth(completer_->popup()->sizeHintForColumn(0) + completer_->popup()->verticalScrollBar()->sizeHint().width());
				completer_->complete(cr);
			}
		}
	}
}
