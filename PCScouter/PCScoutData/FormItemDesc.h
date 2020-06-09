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
