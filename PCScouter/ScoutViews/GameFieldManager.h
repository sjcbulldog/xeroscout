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
#include "ManagerBase.h"
#include <QFile>
#include <QDebug>
#include <string>
#include <list>
#include <memory>


class GameField;

class GameFieldManager : public ManagerBase
{
public:
	GameFieldManager();
	virtual ~GameFieldManager();

	std::list<std::string> getNames();
	std::list<std::shared_ptr<GameField>> getFields();

	bool exists(const std::string& name);
	bool hasFile(QString filename);

	std::shared_ptr<GameField> getFieldByName(const std::string& name);
	std::shared_ptr<GameField> getDefaultField();

	void convert(const std::string& units);

	static constexpr const char* gameTag = "game";
	static constexpr const char* imageTag = "field-image";
	static constexpr const char* cornersTag = "field-corners";
	static constexpr const char* topLeftCornerTag = "top-left";
	static constexpr const char* bottomRightCornerTag = "bottom-right";
	static constexpr const char* sizeTag = "field-size";
	static constexpr const char* unitsTag = "field-unit";

protected:
	virtual bool checkCount() {
		return fields_.size() > 0;
	}

private:
	bool processJSONFile(QFile& file);

private:
	std::list<std::shared_ptr<GameField>> fields_;
};

