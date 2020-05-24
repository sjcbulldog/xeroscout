#include "catch.h"
#include "ExprLibTestContext.h"
#include <Expr.h>
#include <QDebug>

using namespace xero::expr;

TEST_CASE("ExprSimpleSubstractTest")
{
	SECTION("Add Constants")
	{
		ExprLibTestContext ctxt;
		QVariant v;
		QString err;
		Expr e;
		bool ret;

		ret = e.parse(ctxt, "4-3", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Int);
		REQUIRE(v.toInt() == 1);

		ret = e.parse(ctxt, "  21  -  13   ", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Int);
		REQUIRE(v.toInt() == 8);

		ret = e.parse(ctxt, "6.912-1.234", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Double);
		REQUIRE_THAT(v.toDouble(), Catch::Matchers::Floating::WithinAbsMatcher(5.678, 1e-6));

		ret = e.parse(ctxt, "1.1e2 - 1e1  ", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Double);
		REQUIRE_THAT(v.toDouble(), Catch::Matchers::Floating::WithinAbsMatcher(100.0, 1e-6));
	}

	SECTION("Add Variables")
	{
		ExprLibTestContext ctxt;
		QVariant v;
		QString err;
		Expr e;
		bool ret;

		ctxt.addVar("avar", QVariant(5));
		ctxt.addVar("bvar", QVariant(8));
		ctxt.addVar("cvar", QVariant(1.2));
		ctxt.addVar("dvar", QVariant(2.4));

		ret = e.parse(ctxt, "avar-bvar", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Int);
		REQUIRE(v.toInt() == -3);

		ret = e.parse(ctxt, "cvar    -         dvar", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Double);
		REQUIRE_THAT(v.toDouble(), Catch::Matchers::Floating::WithinAbsMatcher(-1.2, 1e-6));
	}
}
