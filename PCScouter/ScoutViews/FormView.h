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
#include "ViewBase.h"
#include "ScoutingForm.h"
#include "ScoutingDataMap.h"
#include "FormInstance.h"
#include "ImageManager.h"
#include <QTabWidget>
#include <QLabel>
#include <QPushButton>

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
				enum class FormType
				{
					Team,
					Match
				};

			public:
				FormView(xero::scouting::datamodel::ImageManager &images, QString name, QString title, QColor c, 
					FormType formtype_, xero::scouting::datamodel::Alliance a, QWidget* parent = Q_NULLPTR);
				virtual ~FormView();

				void clearView();
				void refreshView();

				void extractData(xero::scouting::datamodel::ScoutingDataMapPtr ptr);
				void assignData(xero::scouting::datamodel::ConstScoutingDataMapPtr ptr);

				QString getType() const {
					return form_->formType();
				}

			private:
				void setScoutingForm(std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form);
				void createFlowSection(std::shared_ptr<const xero::scouting::datamodel::FormSection> section);
				void createGridSection(std::shared_ptr<const xero::scouting::datamodel::FormSection> section);

			private:
				QTabWidget* tabs_;
				QString title_txt_;
				QString name_;
				QLabel* titles_;

				FormType formtype_;
				xero::scouting::datamodel::Alliance alliance_;

				QPushButton* bigger_;
				QPushButton* smaller_;

				std::shared_ptr<const xero::scouting::datamodel::ScoutingForm> form_;
				std::shared_ptr<xero::scouting::datamodel::FormInstance> instance_;

				xero::scouting::datamodel::ImageManager& images_;
			};
		}
	}
}
