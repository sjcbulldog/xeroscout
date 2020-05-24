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

#include "scoutviews_global.h"
#include "ViewBase.h"
#include "ScoutingForm.h"
#include "ScoutingDataMap.h"
#include "FormInstance.h"
#include <QTabWidget>
#include <QLabel>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class SCOUTVIEWS_EXPORT FormView : public QWidget, public ViewBase
			{
				Q_OBJECT

			public:
				FormView(QString name, QString title, QColor c, QWidget* parent = Q_NULLPTR);
				virtual ~FormView();

				void setScoutingForm(std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form);
				void clearView();
				void refreshView() {
				}

				void extractData(xero::scouting::datamodel::ScoutingDataMapPtr ptr);
				void assignData(xero::scouting::datamodel::ConstScoutingDataMapPtr ptr);

				QString getType() const {
					return form_->formType();
				}

			protected:

			private:
				void createSection(std::shared_ptr<const xero::scouting::datamodel::FormSection> section);

			private:
				QTabWidget* tabs_;
				QString title_txt_;
				QString name_;
				QLabel* titles_;

				std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form_;
				std::shared_ptr<xero::scouting::datamodel::FormInstance> instance_;
			};
		}
	}
}
