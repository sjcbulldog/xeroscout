//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
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

#include "DataCollection.h"
#include "FieldDesc.h"
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
			class ImageSupplier;
			class FormItemDisplay;

			class FormItemDesc
			{
			public:
				static constexpr const char* FieldSeperator = "__";

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

				void setAlliance(const QString& a) {
					alliance_ = a;
				}

				const QString& alliance() const {
					return alliance_;
				}

				virtual std::vector<std::shared_ptr<FieldDesc>> getFields() const {
					return fields_;
				}

				std::shared_ptr<const FieldDesc> getField(const QString &name) const {
					for (auto field : fields_)
					{
						if (field->name() == name)
							return field;
					}

					return nullptr;
				}

				bool hasField(const QString& name) const {
					for (auto field : fields_)
					{
						if (field->name() == name)
							return true;
					}

					return false;
				}

				virtual FormItemDisplay* createDisplay(ImageSupplier& images, QWidget* parent) const = 0;

				static QString genComplexName(const QString& tag, const QString& field) {
					return tag + FormItemDesc::FieldSeperator + field;
				}

			protected:
				const QString& basetag() {
					return base_tag_;
				}

				void addField(std::shared_ptr<FieldDesc> desc) {
					fields_.push_back(desc);
				}

				std::shared_ptr<FieldDesc> findField(const QString& name) {
					for (auto f : fields_)
					{
						if (f->name() == name)
							return f;
					}

					return nullptr;
				}

			private:
				QString base_tag_;
				QString display_;
				QString alliance_;
				std::vector<std::shared_ptr<FieldDesc>> fields_;
			};
		}
	}
}
