#include "DataSetExprContext.h"
#include "ScoutingDataSet.h"
#include "ExprNode.h"
#include "ExprEvalException.h"

using namespace xero::expr;

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			DataSetExprContext::DataSetExprContext(ScoutingDataSet& ds, const std::vector<int> &rows) : ds_(ds), rows_(rows)
			{
				current_row_ = -1;
			}

			DataSetExprContext::~DataSetExprContext()
			{
			}

			bool DataSetExprContext::isValidVariable(const QString& name)
			{
				int col = ds_.getColumnByName(name);
				return col != -1;
			}

			QVariant DataSetExprContext::getVariable(const QString& name)
			{
				QVariant ret;
				int col = ds_.getColumnByName(name);

				if (col == -1)
				{
					QString msg = "expression references column '" + name + "' which does not exist";
					ExprEvalException ex(msg.toStdString().c_str());
					throw ex;
				}

				if (rows_.size() == 0)
				{
					QString msg = "expression applied to a selection which matched no rows";
					ExprEvalException ex(msg.toStdString().c_str());
					throw ex;
				}

				if (current_row_ == -1)
				{
					ret = ds_.get(rows_[0], col);

					for (int i = 1; i < rows_.size(); i++)
					{
						QVariant v = ds_.get(rows_[i], col);
						if (ret != v)
						{
							QString msg = "expression references column '" + name + "' directly by name which varies across the matched rows";
							ExprEvalException ex(msg.toStdString().c_str());
							throw ex;
						}
					}
				}
				else
				{
					int row = current_row_;
					if (row == -1)
						row = rows_[0];

					ret = ds_.get(row, col);
				}

				return ret;
			}

			bool DataSetExprContext::isValidFunction(const QString& name)
			{
				return name == "sum" || name == "average" || name == "avg" || name == "each" || name == "abs";
			}

			QVariant DataSetExprContext::evalFunction(const QString& name, std::vector<std::shared_ptr<ExprNode>> args)
			{
				QVariant ret;

				if (name == "abs")
				{
					if (args.size() != 1)
					{
						QString msg = "invalid number of arguments for function '" + name + "', expected 1, got " + QString::number(args.size());
						ExprEvalException ex(msg.toStdString().c_str());
						throw ex;
					}

					ret = args[0]->eval(*this);
					if (ret.type() != QVariant::Int && ret.type() != QVariant::Double && ret.type() != QVariant::LongLong)
					{
						QString msg = "invalid arguments type for function '" + name + "', expected integer or double";
						ExprEvalException ex(msg.toStdString().c_str());
						throw ex;
					}

					if (ret.type() == QVariant::Type::Int && ret.toInt() < 0)
						ret = QVariant(-ret.toInt());
					else if (ret.type() == QVariant::Type::Double && ret.toDouble() < 0)
						ret = QVariant(-ret.toInt());
					if (ret.type() == QVariant::Type::LongLong && ret.toInt() < 0)
						ret = QVariant(-ret.toInt());

				}
				else if (name == "sum" || name == "avg" || name == "average")
				{
					if (args.size() != 1)
					{
						QString msg = "invalid number of arguments for function '" + name + "', expected 1, got " + QString::number(args.size());
						ExprEvalException ex(msg.toStdString().c_str());
						throw ex;
					}

					double sum = 0.0;
					for (int i = 0; i < rows_.size(); i++)
					{
						current_row_ = rows_[i];
						QVariant v = args[0]->eval(*this);
						current_row_ = -1;
						if (!v.isValid())
							return v;

						sum += v.toDouble();
					}


					if (name == "sum")
					{
						ret = QVariant(sum);
					}
					else if (name == "average" || name == "avg")
					{
						ret = QVariant(sum / static_cast<double>(rows_.size()));
					}
				}
				else if (name == "pick")
				{
					if (args.size() != 2)
					{
						QString msg = "invalid number of arguments for function '" + name + "', expected 2, got " + QString::number(args.size());
						ExprEvalException ex(msg.toStdString().c_str());
						throw ex;
					}

					QVariant which = args[1]->eval(*this);
					if (which.type() != QVariant::Type::Int)
						return ret;

					int index = which.toInt();
					if (index < 0 || index >= rows_.size())
						return ret;

					current_row_ = rows_[index];
					ret = args[0]->eval(*this);
					current_row_ = -1;
				}

				return ret;
			}
		}
	}
}
