#pragma once

#include "exprlib_global.h"
#include "ExprContext.h"
#include "ExprOperator.h"
#include "ExprNode.h"
#include <QVariant>
#include <QString>
#include <stack>

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

			void reduce(std::stack<std::shared_ptr<ExprNode>>& operands, std::stack<std::shared_ptr<ExprOperator>>& operators);
			void printStacks(std::stack<std::shared_ptr<ExprNode>>& operands, std::stack<std::shared_ptr<ExprOperator>>& operators);

		private:
			std::shared_ptr<ExprNode> top_;
		};
	}
}