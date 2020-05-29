#pragma once

#include <QString>
#include <QVariant>

class TestDataInjector
{
public:
	bool hasData(const QString& name);
	QVariant data(const QString& name);

	static TestDataInjector& getInstance();

	bool parseData(const QString& str);

private:
	TestDataInjector();
	virtual ~TestDataInjector();

private:
	std::map<QString, QVariant> data_;

	static TestDataInjector* theOne;
};

