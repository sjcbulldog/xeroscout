#pragma once

#include "scoutviews_global.h"
#include "DataModelMatch.h"
#include "ViewBase.h"
#include <QWidget>
#include <QTreeWidget>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT DataMergeListWidget : public QWidget, public ViewBase
			{
				Q_OBJECT 

			public:
				DataMergeListWidget(QWidget* parent = 0);
				virtual ~DataMergeListWidget();

				virtual void clearView() ;
				virtual void refreshView() ;

			signals:
				void mergeMatch(const QString& key, xero::scouting::datamodel::DataModelMatch::Alliance c, int slot);
				void mergePit(const QString& key);

			private:
				void doubleClicked(QTreeWidgetItem* item, int col);

			private:
				QTreeWidget* tree_;
			};
		}
	}
}
