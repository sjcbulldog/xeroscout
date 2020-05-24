#pragma once

#include "pcscoutdata_global.h"
#include "ScoutingDataSet.h"
#include <QString>
#include <QVariant>
#include <vector>
#include <map>
#include <memory>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{

			class PCSCOUTDATA_EXPORT TeamDataSummary
			{
			public:
				class Field
				{
				public:
					Field(const QString& name) {
						name_ = name;
						empty_ = 0;
					}

					void addEmpty() {
						empty_++;
					}

					int empty() {
						return empty_;
					}

					const QString& name() {
						return name_;
					}

					virtual QString toString() = 0;
					virtual QString toHTML() = 0;
					virtual QVariant::Type type() = 0;

				private:
					QString name_;
					int empty_;
				};

				class IntField : public Field
				{
				public:
					IntField(const QString& name) : Field(name) {
						count_ = 0;
						total_ = 0;
						minv_ = std::numeric_limits<int>::max();
						maxv_ = std::numeric_limits<int>::min();
					}

					void addSample(double s) {
						count_++;
						total_ += s;
						minv_ = qMin(minv_, s);
						maxv_ = qMax(maxv_, s);
					}

					QString toString() override {
						QString ret = name() + ":";
						ret += " average " + QString::number((double)total_ / (double)count_);
						ret += " min " + QString::number(minv_);
						ret += " max " + QString::number(maxv_);
						ret += "\n";

						return ret;
					}

					QString toHTML() override {
						QString ret = name() + ":<br>";
						ret += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;average " + QString::number((double)total_ / (double)count_) + "<br>";
						ret += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;min " + QString::number(minv_) + "<br>";
						ret += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;max " + QString::number(maxv_) + "<br>";
						if (empty() > 0)
							ret += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;empty " + QString::number(empty()) + "<br>";
						return ret;
					}

					virtual QVariant::Type type() {
						return QVariant::Int;
					}

					double average() {
						return (double)total_ / (double)count_;
					}

				private:
					int count_;
					double total_;
					double minv_;
					double maxv_;
				};

				class BooleanField : public Field
				{
				public:
					BooleanField(const QString& name) : Field(name) {
						true_ = 0;
						false_ = 0;
					}

					void incr(bool b) {
						if (b)
							true_++;
						else
							false_++;
					}

					QString toString() override {
						QString ret = name() + ":";

						int pcnt = static_cast<int>((double)true_ / (double)(true_ + false_) * 100.0);
						ret += " true " + QString::number(true_);
						ret += " (" + QString::number(pcnt) + "%)";

						pcnt = static_cast<int>((double)false_ / (double)(true_ + false_) * 100.0);
						ret += " false " + QString::number(false_);
						ret += " (" + QString::number(pcnt) + "%)";
						ret += "\n";

						return ret;
					}

					QString toHTML() override {
						QString ret = name() + "<br>";

						int total = 0;

						int pcnt = static_cast<int>((double)true_ / (double)(true_ + false_) * 100.0);
						ret += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;  true " + QString::number(true_);
						ret += " (" + QString::number(pcnt) + "%)<br>";

						pcnt = static_cast<int>((double)false_ / (double)(true_ + false_) * 100.0);
						ret += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;  false " + QString::number(false_);
						ret += " (" + QString::number(pcnt) + "%)<br>";

						if (empty() > 0)
							ret += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;empty " + QString::number(empty()) + "<br>";

						return ret;
					}

					virtual QVariant::Type type() {
						return QVariant::Bool;
					}

					double average() {
						return (double)true_ / (double)(true_ + false_);
					}

				private:
					int true_;
					int false_;
				};

				class ChoiceField : public Field
				{
				public:
					ChoiceField(const QString& name) : Field(name) {
						total_ = 0;
					}

					void incrChoice(const QString& choice) {
						auto it = counts_.find(choice);
						if (it == counts_.end())
						{
							counts_.insert(std::make_pair(choice, 1));
						}
						else
						{
							counts_[choice]++;
						}

						total_++;
					}

					QString toString() override {
						QString ret = name() + ":";

						for (auto pair : counts_)
						{
							ret += " " + pair.first + " " + QString::number(pair.second);
							int pcnt = static_cast<int>((double)pair.second / (double)total_ * 100.0);
							ret += " (" + QString::number(pcnt) + "%)";
						}
						ret += "\n";

						return ret;
					}

					QString toHTML() override {
						QString ret = name() + "<br>";

						for (auto pair : counts_)
						{
							ret += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + pair.first + " " + QString::number(pair.second);
							int pcnt = static_cast<int>((double)pair.second / (double)total_ * 100.0);
							ret += " (" + QString::number(pcnt) + "%)<br>";
						}

						if (empty() > 0)
							ret += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;empty " + QString::number(empty()) + "<br>";

						return ret;
					}

					QStringList choices() {
						QStringList list;

						for (auto pair : counts_)
							list.push_back(pair.first);

						return list;
					}

					virtual QVariant::Type type() {
						return QVariant::String;
					}

					double average(const QString& which) {
						auto it = counts_.find(which);
						if (it == counts_.end())
							return 0.0;

						return (double)it->second / (double)total_ * 100.0;
					}

				private:
					int total_;
					std::map<QString, int> counts_;
				};

			public:

				TeamDataSummary();
				virtual ~TeamDataSummary();

				void clear();
				void processDataSet(const ScoutingDataSet& dataset);
				void processScoutingData(ScoutingDataMapPtr scout);

				const std::vector<std::shared_ptr<Field>>& fields() const { return fields_; }

				void removeField(const QString& name) {
					for (int i = 0; i < fields_.size(); i++) {
						if (fields_[i]->name() == name)
						{
							auto it = fields_.begin();
							std::advance(it, i);
							fields_.erase(it);
							break;
						}
					}
				}

			private:
				std::vector<std::shared_ptr<Field>> fields_;
			};
		}
	}
}
