#pragma once
#include <QCompleter>
#include <QLineEdit>

namespace xero
{
    namespace scouting
    {
        namespace views
        {
            class DelimitedCompleter : public QCompleter
            {
                Q_OBJECT

            public:
                DelimitedCompleter(QLineEdit* parent, char delimiter);
                DelimitedCompleter(QLineEdit* parent, char delimiter, QAbstractItemModel* model);
                DelimitedCompleter(QLineEdit* parent, char delimiter, const QStringList& list);
                QString pathFromIndex(const QModelIndex& index) const;
                QStringList splitPath(const QString& path) const;

            private:
                char delimiter;
                mutable int cursor_pos = -1;

                void connectSignals();

            private slots:
                void onActivated(const QString& text);
                void onCursorPositionChanged(int old_pos, int new_pos);
            };
        }
    }
}
