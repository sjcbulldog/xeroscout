//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
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
