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
