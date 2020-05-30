#include "FormInstance.h"
#include <QDebug>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			FormInstance::FormInstance(std::shared_ptr<const ScoutingForm> form)
			{
				form_ = form;
			}

			FormInstance::~FormInstance()
			{
			}

			void FormInstance::extract(ScoutingDataMapPtr data)
			{
				data->clear();

				for (auto item : items_)
				{
					auto coll = item.second->getValues();
					for (int i = 0; i < coll.count(); i++)
					{
						QString tag = coll.tag(i);
						data->insert_or_assign(tag, coll.data(i));
					}
				}
			}

			void FormInstance::assign(ConstScoutingDataMapPtr data)
			{
				for (auto item : items_)
				{
					auto fieldlist = item.second->desc()->getFields();
					DataCollection d;

					for (auto f : fieldlist)
					{
						auto it = data->find(f->name());
						if (it != data->end())
						{
							d.add(it->first, it->second);
						}
					}

					if (d.count() == item.second->desc()->getFields().size())
					{
						item.second->setValues(d);
					}
				}
			}
		}
	}
}
