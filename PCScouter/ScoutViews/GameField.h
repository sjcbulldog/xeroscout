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
#include "UnitConverter.h"
#include <QPointF>
#include <string>
#include <QDir>
#include <QFile>
#include <QFileInfo>

namespace xero
{
	namespace scouting
	{
		namespace views
		{
			class GameField
			{
			public:
				GameField(const QDir& path, const std::string& name, const std::string& image, const std::string& units,
					QPointF topleft, QPointF bottomright,
					QPointF size) {
					path_ = path;
					name_ = name;
					image_ = image;
					units_ = units;
					top_left_ = topleft;
					bottom_right_ = bottomright;
					size_ = size;
				}

				~GameField() {
				}

				const std::string& getName() const {
					return name_;
				}

				QString getImageFile() {
					QFileInfo info(path_, image_.c_str());
					return info.absoluteFilePath();
				}

				QPointF getTopLeft() const {
					return top_left_;
				}

				QPointF getBottomRight() const {
					return bottom_right_;
				}

				QPointF getSize() const {
					return size_;
				}

				void convert(const std::string& units) {
					if (units != units_)
					{
						double x = UnitConverter::convert(size_.x(), units_, units);
						double y = UnitConverter::convert(size_.y(), units_, units);

						size_ = QPointF(x, y);

						units_ = units;
					}
				}

			private:
				std::string name_;
				std::string image_;
				std::string units_;
				QPointF top_left_;
				QPointF bottom_right_;
				QPointF size_;
				QDir path_;
			};
		}
	}
}
