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
