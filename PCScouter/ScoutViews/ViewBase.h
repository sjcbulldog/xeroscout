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
				ViewBase(const QString &name) : empty_("empty") {
					name_ = name;
					need_refresh_ = true;
				}

				virtual ~ViewBase() {
				}

				virtual void setYear(const QString& year) {
				}

				const QString& name() const {
					return name_;
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

				virtual bool hasDataSet() {
					return false;
				}

				virtual xero::scouting::datamodel::ScoutingDataSet& dataset() {
					assert(false);
					return empty_;
				}

				virtual void clearView() = 0;
				virtual void refreshView() = 0;
				virtual void setTablet(const QString& tablet) {
				}
				virtual void setKey(const QString& key) {
				}

			private:
				std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> model_;
				bool need_refresh_;
				QString name_;
				xero::scouting::datamodel::ScoutingDataSet empty_;
			};
		}
	}
}

