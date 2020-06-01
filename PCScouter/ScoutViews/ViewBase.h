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

#include "ScoutingDataModel.h"
#include <QDebug>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class ViewBase
			{
			public:
				ViewBase(const QString &name) {
					name_ = name;
					need_refresh_ = true;
				}

				virtual ~ViewBase() {
				}

				virtual void setDataModel(std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> model) {
					model_ = model;
				}

				std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dataModel() {
					return model_;
				}

				virtual void setNeedRefresh() {
					need_refresh_ = true;
				}

				virtual void clearNeedRefresh() {
					need_refresh_ = false;
				}

				virtual bool needsRefresh() {
					return need_refresh_;
				}

				virtual void madeActive() {

				}

				virtual void clearView() = 0;
				virtual void refreshView() = 0;
				virtual void setTablet(const QString& tablet) {
				}

			private:
				std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> model_;
				bool need_refresh_;
				QString name_;
			};
		}
	}
}

