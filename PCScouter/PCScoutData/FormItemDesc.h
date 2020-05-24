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

#include "GameRandomProfile.h"
#include "DataCollection.h"
#include <QString>
#include <QWidget>
#include <QVariant>
#include <QRandomGenerator>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class FormItemDisplay;

			class FormItemDesc
			{
			public:
				FormItemDesc(const QString& display, const QString& tag) {
					display_ = display;
					base_tag_ = tag;
				}

				virtual ~FormItemDesc() {
				}

				const QString& display() const {
					return display_;
				}

				const QString& tag() const {
					return base_tag_;
				}

				virtual std::vector<std::pair<QString, QVariant::Type>> getFields() const {
					return fields_;
				}

				virtual DataCollection random(GameRandomProfile &profile) const = 0;
				virtual FormItemDisplay* createDisplay(QWidget* parent) const = 0;

			protected:
				const QString& basetag() {
					return base_tag_;
				}

				void addField(const std::pair<QString, QVariant::Type>& field) {
					fields_.push_back(field);
				}

			private:
				QString base_tag_;
				QString display_;
				std::vector<std::pair<QString, QVariant::Type>> fields_;
			};
		}
	}
}
