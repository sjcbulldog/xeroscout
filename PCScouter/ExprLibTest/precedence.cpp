#include "catch.h"
#include "ExprLibTestContext.h"
#include <Expr.h>
#include <QDebug>

using namespace xero::expr;

TEST_CASE("ExprPrecedence")
{
	SECTION("TestOne")
	{
		ExprLibTestContext ctxt;
		QVariant v;
		QString err;
		Expr e;
		bool ret;

		ret = e.parse(ctxt, "1 + 2 * 3", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Int);
		REQUIRE(v.toInt() == 7);

		ret = e.parse(ctxt, "1 * 2 + 3", err);
		REQUIRE(ret == true);
		REQUIRE(err.length() == 0);

		v = e.eval(ctxt);
		REQUIRE(v.type() == QVariant::Type::Int);
		REQUIRE(v.toInt() == 5);
	}
}
