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
