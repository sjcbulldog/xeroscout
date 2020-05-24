#include "TeamDataSummary.h"
#include <QVariant>
#include <cassert>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			TeamDataSummary::TeamDataSummary()
			{
			}

			TeamDataSummary::~TeamDataSummary()
			{
			}

			void TeamDataSummary::clear()
			{
				fields_.clear();
			}

			void TeamDataSummary::processDataSet(const ScoutingDataSet& dataset)
			{
				for (int col = 0; col < dataset.columnCount(); col++) {
					QVariant::Type t = dataset.colType(col);

					if (dataset.isColumnBool(col))
					{
						auto field = std::make_shared<BooleanField>(dataset.colHeader(col));
						fields_.push_back(field);
						for (int row = 0; row < dataset.rowCount(); row++)
						{
							QVariant v = dataset.get(row, col);
							if (v.isValid())
							{
								field->incr(v.toInt() ? true : false);
							}
							else
							{
								field->addEmpty();
							}
						}
					}
					else if (t == QVariant::Type::Int)
					{
						auto field = std::make_shared<IntField>(dataset.colHeader(col));
						fields_.push_back(field);
						for (int row = 0; row < dataset.rowCount(); row++)
						{
							QVariant v = dataset.get(row, col);
							if (v.isValid())
							{
								field->addSample(v.toInt());
							}
							else
							{
								field->addEmpty();
							}
						}
					}
					else if (t == QVariant::Type::Double)
					{
						auto field = std::make_shared<IntField>(dataset.colHeader(col));
						fields_.push_back(field);
						for (int row = 0; row < dataset.rowCount(); row++)
						{
							QVariant v = dataset.get(row, col);
							if (v.isValid())
							{
								field->addSample(v.toDouble());
							}
							else
							{
								field->addEmpty();
							}
						}
					}
					else if (t == QVariant::Type::String)
					{
						auto field = std::make_shared<ChoiceField>(dataset.colHeader(col));
						fields_.push_back(field);
						for (int row = 0; row < dataset.rowCount(); row++)
						{
							QVariant v = dataset.get(row, col);
							if (v.isValid())
							{
								assert(v.type() == t);
								if (t == QVariant::Type::Bool)
								{
									if (v.toBool())
										field->incrChoice("TRUE");
									else
										field->incrChoice("FALSE");
								}
								else
								{
									field->incrChoice(v.toString());
								}
							}
							else
							{
								field->addEmpty();
							}
						}
					}
					else
					{
						assert(false);
					}
				}
			}

			void TeamDataSummary::processScoutingData(ScoutingDataMapPtr scout)
			{
				for (auto pair : *scout)
				{
					if (pair.second.type() == QVariant::Int)
					{
						auto f = std::make_shared<IntField>(pair.first);
						f->addSample(pair.second.toInt());
						fields_.push_back(f);
					}
					else if (pair.second.type() == QVariant::Double)
					{
						auto f = std::make_shared<IntField>(pair.first);
						f->addSample(pair.second.toDouble());
						fields_.push_back(f);
					}
					else if (pair.second.type() == QVariant::Bool)
					{
						auto f = std::make_shared<ChoiceField>(pair.first);
						if (pair.second.toBool())
							f->incrChoice("TRUE");
						else
							f->incrChoice("FALSE");
						fields_.push_back(f);
					}
					else if (pair.second.type() == QVariant::String)
					{
						auto f = std::make_shared<ChoiceField>(pair.first);
						f->incrChoice(pair.second.toString());
						fields_.push_back(f);
					}
				}
			}
		}
	}
}
