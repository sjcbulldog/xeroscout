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


#include "FormItemDesc.h"
#include <QString>
#include <list>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			class FormSection
			{
			public:
				FormSection(const QString& name) {
					name_ = name;
				}

				void getImages(QStringList& imlist) {
					for (auto item : items_) {
						item->getImages(imlist);
					}
				}

				const QString& name() const {
					return name_;
				}

				void addItem(std::shared_ptr<FormItemDesc> item) {
					items_.push_back(item);
				}

				std::shared_ptr<FormItemDesc> itemByName(const QString &name) const {
					for (auto item : items_)
					{
						if (item->tag() == name)
							return item;
					}

					return nullptr;
				}

				std::list<std::shared_ptr<const FormItemDesc>> items() const {
					std::list<std::shared_ptr<const FormItemDesc>> ret;

					for (auto i : items_)
						ret.push_back(i);

					return ret;
				}

			private:
				QString name_;
				std::list<std::shared_ptr<FormItemDesc>> items_;
			};

		}
	}
}
