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
#include "Translation2d.h"
#include <string>
#include <QDir>
#include <QFile>
#include <QFileInfo>

class GameField
{
public:
	GameField(const QDir& path, const std::string& name, const std::string& image, const std::string& units,
		xero::paths::Translation2d topleft, xero::paths::Translation2d bottomright,
		xero::paths::Translation2d size) {
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

	xero::paths::Translation2d getTopLeft() const {
		return top_left_;
	}

	xero::paths::Translation2d getBottomRight() const {
		return bottom_right_;
	}

	xero::paths::Translation2d getSize() const {
		return size_;
	}

	void convert(const std::string& units) {
		if (units != units_)
		{
			double x = xero::paths::UnitConverter::convert(size_.getX(), units_, units);
			double y = xero::paths::UnitConverter::convert(size_.getY(), units_, units);

			size_ = xero::paths::Translation2d(x, y);

			units_ = units;
		}
	}

private:
	std::string name_;
	std::string image_;
	std::string units_;
	xero::paths::Translation2d top_left_;
	xero::paths::Translation2d bottom_right_;
	xero::paths::Translation2d size_;
	QDir path_;
};

