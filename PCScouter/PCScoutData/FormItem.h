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
			class FormItem
			{
			public:
				FormItem(const QString& display, const QString& tag) {
					display_ = display;
					tag_ = tag;
				}

				virtual ~FormItem() {
				}

				const QString& display() const {
					return display_;
				}

				const QString& tag() const {
					return tag_;
				}

				virtual QVariant::Type dataType() const = 0;

				virtual QWidget* createWidget(const QString& name, QWidget* parent) const = 0;

				virtual void destroyWidget(const QString& name) const = 0;

				virtual QVariant getValue(const QString& name) const = 0;

				virtual void setValue(const QString& name, const QVariant& v) const = 0;

				virtual QVariant random(GameRandomProfile &profile) const = 0;

			private:
				QString tag_;
				QString display_;
			};

		}
	}
}
