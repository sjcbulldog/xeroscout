#pragma once

#include "pcscoutdata_global.h"
#include "ExprContext.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ScoutingDataSet;

			class PCSCOUTDATA_EXPORT DataSetExprContext : public xero::expr::ExprContext
			{
			public:
				DataSetExprContext(ScoutingDataSet& ds, const std::vector<int>& rows);
				virtual ~DataSetExprContext();

				virtual bool isValidVariable(const QString& name);
				virtual QVariant getVariable(const QString& name);

				virtual bool isValidFunction(const QString& name);
				virtual QVariant evalFunction(const QString &name, std::vector<std::shared_ptr<xero::expr::ExprNode>> args);

			private:
				ScoutingDataSet& ds_;
				const std::vector<int>& rows_;
				int current_row_;
			};
		}
	}
}
