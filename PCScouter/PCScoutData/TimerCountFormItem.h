#pragma once
#include "FormItemDesc.h"
#include "TimerCountItemDisplay.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class TimerCountFormItem : public FormItemDesc
			{
			public:
				constexpr static const char* CompletedTimeField = "completedtime";
				constexpr static const char* CompletedCountField = "completedcount";
				constexpr static const char* AbandonedTimeField =  "abandonedtime";
				constexpr static const char* AbandonedCountField = "abandonedcount";

			public:
				TimerCountFormItem(const QString& display, const QString& tag, int minv, int maxv) : FormItemDesc(display, tag)
				{
					addField(std::make_pair(genComplexName(tag, CompletedTimeField), QVariant::Double));
					addField(std::make_pair(genComplexName(tag, CompletedCountField), QVariant::Int));
					addField(std::make_pair(genComplexName(tag, AbandonedTimeField), QVariant::Double));
					addField(std::make_pair(genComplexName(tag, AbandonedCountField), QVariant::Int));
					minv_ = minv;
					maxv_ = maxv;
				}

				virtual ~TimerCountFormItem()
				{
				}

				virtual DataCollection random(GameRandomProfile& profile) const
				{
					DataCollection d;

					QVariant v = profile.generateRandomInteger(tag() + FormItemDesc::FieldSeperator + CompletedCountField, minv_, maxv_);
					d.add(genComplexName(tag(), CompletedCountField), v);

					v = profile.generateRandomReal(tag() + ":" + CompletedTimeField, 0.0, 180.0);
					d.add(genComplexName(tag(), CompletedTimeField), v);

					v = profile.generateRandomInteger(tag() + ":" + AbandonedCountField, minv_, maxv_);
					d.add(genComplexName(tag(), AbandonedCountField), v);

					v = profile.generateRandomReal(tag() + ":" + AbandonedTimeField, 0.0, 180.0);
					d.add(genComplexName(tag(), AbandonedTimeField), v);

					return d;
				}

				virtual FormItemDisplay* createDisplay(QWidget* parent) const
				{
					return new TimerCountItemDisplay(this, parent);
				}

			private:
				int minv_;
				int maxv_;
			};
		}
	}
}

