//
// Copyright 2020 by Jack W. (Butch) Griffin
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
