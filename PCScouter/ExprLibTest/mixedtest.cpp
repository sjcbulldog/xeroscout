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
