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

#include "GraphDescriptor.h"
#include <QJsonArray>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			GraphDescriptor::GraphDescriptor(const QString &name)
			{
				name_ = name;
			}

			GraphDescriptor::GraphDescriptor()
			{
			}

			GraphDescriptor::~GraphDescriptor()
			{
			}

			std::shared_ptr<GraphDescriptor::GraphPane> GraphDescriptor::addPane(const QString& title)
			{
				auto pane = std::make_shared<GraphDescriptor::GraphPane>(title);
				panes_.push_back(pane);

				return pane;
			}

			void GraphDescriptor::deletePane(std::shared_ptr<GraphDescriptor::GraphPane> pane)
			{
				auto it = std::find(panes_.begin(), panes_.end(), pane);
				if (it != panes_.end())
					panes_.erase(it);
			}

			QJsonObject GraphDescriptor::generateJSON() const
			{
				QJsonObject obj;
				QJsonArray panes;

				obj["name"] = name_;

				for (auto pane : panes_)
					panes.push_back(pane->generateJSON());

				obj["panes"] = panes;

				return obj;
			}
		}
	}
}
