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

#include "Alliance.h"
#include "DataCollection.h"
#include "FieldDesc.h"
#include <QString>
#include <QtWidgets/QWidget>
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
					row_ = -1;
					col_ = -1;
					width_ = -1;
					height_ = -1;
				}

				virtual ~FormItemDesc() {
				}

				bool hasSize() const {
					return width_ != -1 && height_ != -1;
				}

				bool hasPos() const {
					return row_ != -1 && col_ != -1;
				}

				int width() const {
					return width_;
				}

				int height() const {
					return height_;
				}

				int row() const {
					return row_;
				}

				int col() const {
					return col_;
				}

				const QString& display() const {
					return display_;
				}

				const QString& tag() const {
					return base_tag_;
				}

				void setAlliance(Alliance a) {
					alliance_ = a;
				}

				Alliance alliance() const {
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
				virtual void getImages(QStringList& images) {
				}

				static QString genComplexName(const QString& tag, const QString& field) {
					return tag + FormItemDesc::FieldSeperator + field;
				}

				void setPos(int row, int col) {
					row_ = row;
					col_ = col;
				}

				void setSize(int width, int height) {
					width_ = width;
					height_ = height;
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
				int row_;
				int col_;
				int width_;
				int height_;

				QString base_tag_;
				QString display_;
				Alliance alliance_;
				std::vector<std::shared_ptr<FieldDesc>> fields_;
			};
		}
	}
}
