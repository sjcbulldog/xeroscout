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
