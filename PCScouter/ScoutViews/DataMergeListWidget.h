//
// Copyright(C) 2020 by Jack (Butch) Griffin
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see < https://www.gnu.org/licenses/>.
//
//
//
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
//

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
				void mergeMatch(const QString& key, xero::scouting::datamodel::Alliance c, int slot);
				void mergePit(const QString& key);

			private:
				void doubleClicked(QTreeWidgetItem* item, int col);

			private:
				QTreeWidget* tree_;
			};
		}
	}
}
