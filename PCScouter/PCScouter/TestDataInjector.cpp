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