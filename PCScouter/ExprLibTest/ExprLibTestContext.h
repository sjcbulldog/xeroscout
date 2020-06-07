//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
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
