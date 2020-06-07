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

#include "catch.h"
#include "ExprLibTestContext.h"
#include <Expr.h>
#include <QDebug>

using namespace xero::expr;

TEST_CASE("ExprMixedTest")
{
	SECTION("Multiply")
	{
		ExprLibTestContext ctxt;
		QVariant v;
		QString err;
		Expr e;
		bool ret;

		ret = e.parse(ctxt, "2*4", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Int);
		REQUIRE(v.toInt() == 8);

		ret = e.parse(ctxt, "2*4.0e0", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Double);
		REQUIRE_THAT(v.toDouble(), Catch::Matchers::Floating::WithinAbsMatcher(8.0, 1e-6));

		ret = e.parse(ctxt, "1.1 * 2.2 ", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Double);
		REQUIRE_THAT(v.toDouble(), Catch::Matchers::Floating::WithinAbsMatcher(2.42, 1e-6));
	}

	SECTION("Divide")
	{
		ExprLibTestContext ctxt;
		QVariant v;
		QString err;
		Expr e;
		bool ret;

		ret = e.parse(ctxt, "26 / 13", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Int);
		REQUIRE(v.toInt() == 2);

		ret = e.parse(ctxt, "52.0 / 13 ", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Double);
		REQUIRE_THAT(v.toDouble(), Catch::Matchers::Floating::WithinAbsMatcher(4.0, 1e-6));

		ret = e.parse(ctxt, "52 / 13.0 ", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Double);
		REQUIRE_THAT(v.toDouble(), Catch::Matchers::Floating::WithinAbsMatcher(4.0, 1e-6));

		ret = e.parse(ctxt, "52.0 / 13.0 ", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Double);
		REQUIRE_THAT(v.toDouble(), Catch::Matchers::Floating::WithinAbsMatcher(4.0, 1e-6));
	}
}
