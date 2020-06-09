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
#include <QTreeWidget>
#include <list>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace views
		{

			class SCOUTVIEWS_EXPORT MatchViewWidget : public QTreeWidget, public ViewBase
			{
			public:
				MatchViewWidget(const QString &tablet, QWidget* parent);
				virtual ~MatchViewWidget();

				void setData(std::list<std::shared_ptr<const xero::scouting::datamodel::DataModelMatch>> matches);
				void clearView();
				void refreshView();
				void setScoutingField(const QString& key, xero::scouting::datamodel::Alliance c, int slot);

				void setTablet(const QString& tablet) override {
					tablet_ = tablet;
					refreshView();
				}

			private:
				static bool wayToSort(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> one, std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> two);
				static int matchClass(const QString& comp);
				void putOneAll(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> match);
				void putOneOne(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> match);
				int putAllianceData(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> m, QTreeWidgetItem* i, xero::scouting::datamodel::Alliance a, int index);

				QString genStatusText(std::shared_ptr<const xero::scouting::datamodel::DataModelMatch> match, xero::scouting::datamodel::Alliance a, int slot, bool tt);

			private:
				QString tablet_;
				std::list<std::shared_ptr<const xero::scouting::datamodel::DataModelMatch>> matches_;

				static QStringList headers_all_;
				static QStringList headers_one_;
			};

		}
	}
}
