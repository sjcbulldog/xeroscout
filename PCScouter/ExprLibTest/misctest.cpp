#include "catch.h"
#include "ExprLibTestContext.h"
#include <Expr.h>
#include <QDebug>

using namespace xero::expr;

TEST_CASE("ExprMiscTest")
{
	SECTION("Add Constants")
	{
		ExprLibTestContext ctxt;
		QVariant v;
		QString err;
		Expr e;
		bool ret;

		ctxt.addVar("_a123", 1);
		ctxt.addVar("_b123", 2.0);

		ret = e.parse(ctxt, "18", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Int);
		REQUIRE(v.toInt() == 18);

		ret = e.parse(ctxt, "3.141592", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Double);
		REQUIRE(v.toDouble() == 3.141592);

		ret = e.parse(ctxt, "(42)", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Int);
		REQUIRE(v.toInt() == 42);

		ret = e.parse(ctxt, "(42.24)", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Double);
		REQUIRE_THAT(v.toDouble(), Catch::Matchers::Floating::WithinAbsMatcher(42.24, 1e-6));

		ret = e.parse(ctxt, "(1 + 2) * (3 + 4)", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Int);
		REQUIRE(v.toInt() == 21);

		ret = e.parse(ctxt, "(1 * 2) + (3 * 4)", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Int);
		REQUIRE(v.toInt() == 14);
	}
}
