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
					addField(std::make_shared<FieldDesc>(genComplexName(tag, CompletedTimeField), FieldDesc::Type::Double, true));
					addField(std::make_shared<FieldDesc>(genComplexName(tag, CompletedCountField), FieldDesc::Type::Integer, true));
					addField(std::make_shared<FieldDesc>(genComplexName(tag, AbandonedTimeField), FieldDesc::Type::Double, true));
					addField(std::make_shared<FieldDesc>(genComplexName(tag, AbandonedCountField), FieldDesc::Type::Integer, true));
					minv_ = minv;
					maxv_ = maxv;
				}

				virtual ~TimerCountFormItem()
				{
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

