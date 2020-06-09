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

#include "TestDataInjector.h"
#include <cassert>

TestDataInjector* TestDataInjector::theOne = nullptr;

TestDataInjector::TestDataInjector()
{
	assert(theOne == nullptr);
}

TestDataInjector::~TestDataInjector()
{
	assert(theOne == this);
	theOne = nullptr;
}

TestDataInjector& TestDataInjector::getInstance()
{
	if (theOne == nullptr)
		theOne = new TestDataInjector();

	return *theOne;
}

bool TestDataInjector::hasData(const QString& name)
{
	auto it = data_.find(name);
	return it != data_.end();
}

QVariant TestDataInjector::data(const QString& name)
{
	auto it = data_.find(name);
	return it->second;
}

bool TestDataInjector::parseData(const QString& str)
{
	int index = 0;
	QString name, value;

	while (index < str.length())
	{
		if (str[index] == '=')
			break;

		name += str[index++];
	}

	if (index == str.length())
		return false;

	index++;
	QVariant v;

	if (str[index] == '"')
	{
		index++;
		while (index < str.length())
		{
			if (str[index] == '"')
				break;

			value += str[index++];
		}

		if (index == str.length())
			return false;

		index++;
		if (index != str.length())
			return false;

		v = QVariant(value);
	}
	else
	{
		bool ok;

		while (index < str.length())
			value += str[index++];

		if (value.toLower() == "true")
		{
			v = QVariant(true);
		}
		else if (value.toLower() == "false")
		{
			v = QVariant(false);
		}
		else
		{
			int ival = value.toInt(&ok);
			if (ok)
			{
				v = QVariant(ival);
			}
			else
			{
				double dval = value.toDouble(&ok);
				if (!ok)
					return false;

				v = QVariant(dval);
			}
		}
	}

	data_.insert_or_assign(name, v);
	return true;
}