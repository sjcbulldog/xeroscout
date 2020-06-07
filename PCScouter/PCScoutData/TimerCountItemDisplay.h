//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
// 

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
