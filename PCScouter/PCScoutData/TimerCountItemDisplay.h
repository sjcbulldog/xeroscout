//
// Copyright(C) 2020 by Jack (Butch) Griffin
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see < https://www.gnu.org/licenses/>.
//
//
//
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
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
