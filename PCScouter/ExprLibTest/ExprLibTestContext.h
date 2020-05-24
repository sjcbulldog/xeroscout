#pragma once

#include <ExprContext.h>
#include <QString>
#include <QVariant>

class ExprLibTestContext : public xero::expr::ExprContext
{
public:
	ExprLibTestContext() {
	}

	virtual ~ExprLibTestContext() {
	}

	void addVar(const QString& name, const QVariant& v) {
		values_.insert_or_assign(name, v);
	}

	virtual bool isValidVariable(const QString& name) {
		auto it = values_.find(name);
		return it != values_.end();
	}

	virtual QVariant getVariable(const QString& name) {
		QVariant ret;

		auto it = values_.find(name);
		if (it != values_.end())
			ret = it->second;

		return ret;
	}

private:
	std::map<QString, QVariant> values_;
};
