#pragma once

#include "exprlib_global.h"
#include "ExprContext.h"
#include <QVariant>
#include <QString>

namespace xero
{
	namespace expr
	{
		class ExprNode;
		class ExprOperator;
		class ExprContext;

		class EXPRLIB_EXPORT Expr
		{
		public:
			Expr();
			virtual ~Expr();

			QString toString() const;

			bool parse(ExprContext &context, const QString& text, QString& err);

			QStringList allVariables() const;

			QVariant eval(ExprContext&) const;

		private:
			std::shared_ptr<ExprNode> parseSubExpr(ExprContext& context, const QString& txt, int& index, QString& err);
			std::shared_ptr<ExprNode> parseNode(ExprContext& context, const QString& txt, int& index, QString& err);
			std::shared_ptr<ExprOperator> parseOper(ExprContext& context, const QString& txt, int& index, QString& err);
			void skipSpaces(const QString& txt, int& index);

		private:
			std::shared_ptr<ExprNode> top_;
		};
	}
}