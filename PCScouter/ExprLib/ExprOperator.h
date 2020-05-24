#pragma once

#include "ExprNode.h"
#include <vector>

namespace xero
{
	namespace expr
	{
		class ExprOperator : public ExprNode
		{
		public:
			enum class OpType
			{
				Plus,
				Minus,
				Times,
				Divide,
			};

		public:
			ExprOperator(Expr* e, OpType op);
			virtual ~ExprOperator();

			virtual QVariant eval(ExprContext& ctxt);
			virtual QString toString();
			virtual void allVariables(QStringList& list);

			void addNode(std::shared_ptr<ExprNode> node) {
				nodes_.push_back(node);
			}

			int prec();

		private:
			static QVariant plus(const QVariant& v1, const QVariant& v2);
			static QVariant minus(const QVariant& v1, const QVariant& v2);
			static QVariant times(const QVariant& v1, const QVariant& v2);
			static QVariant divide(const QVariant& v1, const QVariant& v2);

		private:
			OpType op_;
			std::vector<std::shared_ptr<ExprNode>> nodes_;
		};
	}
}

