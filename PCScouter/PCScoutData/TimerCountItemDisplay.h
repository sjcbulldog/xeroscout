#pragma once
#include "FormItemDisplay.h"
#include "StartStopWidget.h"
#include <QLabel>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class TimerCountItemDisplay : public FormItemDisplay
			{
			public:
				TimerCountItemDisplay(ImageSupplier& images, const FormItemDesc* desc, QWidget* parent);
				virtual ~TimerCountItemDisplay();

				virtual void setValues(const DataCollection& data);
				virtual DataCollection getValues();

			private:
				void updateDisplay();

				void cycleStarted();
				void cycleCompleted();
				void cycleAbandoned();

				void timerTick();

			private:
				QTimer* timer_;

				double running_time_;
				double completed_time_;
				double abandoned_time_;
				int completed_count_;
				int abandoned_count_;

				StartStopWidget* left_;
				QWidget* right_;
				QLabel* running_time_label_;
				QLabel* completed_time_label_;
				QLabel* completed_count_label_;
				QLabel* abandoned_time_label_;
				QLabel* abandoned_count_label_;
			};
		}
	}
}
