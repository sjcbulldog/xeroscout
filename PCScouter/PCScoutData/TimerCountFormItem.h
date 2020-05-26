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
					addField(std::make_pair(genComplexName(tag, CompletedTimeField), QVariant::Type::Double));
					addField(std::make_pair(genComplexName(tag, CompletedCountField), QVariant::Type::Int));
					addField(std::make_pair(genComplexName(tag, AbandonedTimeField), QVariant::Type::Double));
					addField(std::make_pair(genComplexName(tag, AbandonedCountField), QVariant::Type::Int));
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

					v = profile.generateRandomReal(genComplexName(tag(), CompletedTimeField), 0.0, 180.0);
					d.add(genComplexName(tag(), CompletedTimeField), v);

					v = profile.generateRandomInteger(genComplexName(tag(), AbandonedCountField), minv_, maxv_);
					d.add(genComplexName(tag(), AbandonedCountField), v);

					v = profile.generateRandomReal(genComplexName(tag(), AbandonedTimeField), 0.0, 180.0);
					d.add(genComplexName(tag(), AbandonedTimeField), v);

					return d;
				}

				virtual FormItemDisplay* createDisplay(ImageSupplier& images, QWidget* parent) const
				{
					return new TimerCountItemDisplay(images, this, parent);
				}

			private:
				int minv_;
				int maxv_;
			};
		}
	}
}

