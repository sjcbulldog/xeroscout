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

#include "TimerCountItemDisplay.h"
#include "TimerCountFormItem.h"
#include <QBoxLayout>
#include <QTimer>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			TimerCountItemDisplay::TimerCountItemDisplay(ImageSupplier& images, const FormItemDesc* desc, QWidget* parent) : FormItemDisplay(images, desc, parent)
			{
				timer_ = nullptr;

				QVBoxLayout* vlay = new QVBoxLayout();
				setLayout(vlay);

				QLabel* title = new QLabel(desc->display());
				vlay->addWidget(title);

				QWidget* bottom = new QWidget(this);
				vlay->addWidget(bottom);

				QHBoxLayout* layout = new QHBoxLayout();
				bottom->setLayout(layout);

				QSizePolicy p(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
				bottom->setSizePolicy(p);

				left_ = new StartStopWidget(bottom);
				layout->addWidget(left_);
				connect(left_, &StartStopWidget::cycleStarted, this, &TimerCountItemDisplay::cycleStarted);
				connect(left_, &StartStopWidget::cycleCompleted, this, &TimerCountItemDisplay::cycleCompleted);
				connect(left_, &StartStopWidget::cycleAbandoned, this, &TimerCountItemDisplay::cycleAbandoned);

				right_ = new QWidget(bottom);
				layout->addWidget(right_);

				vlay = new QVBoxLayout();
				right_->setLayout(vlay);

				running_time_label_ = new QLabel(right_);
				vlay->addWidget(running_time_label_);

				completed_time_label_ = new QLabel(right_);
				vlay->addWidget(completed_time_label_);

				completed_count_label_ = new QLabel(right_);
				vlay->addWidget(completed_count_label_);

				abandoned_time_label_ = new QLabel(right_);
				vlay->addWidget(abandoned_time_label_);

				abandoned_count_label_ = new QLabel(right_);
				vlay->addWidget(abandoned_count_label_);

				completed_count_ = 0;
				completed_time_ = 0.0;
				abandoned_count_ = 0;
				abandoned_time_ = 0.0;

				running_time_ = 0.0;

				updateDisplay();
			}

			TimerCountItemDisplay::~TimerCountItemDisplay()
			{
			}

			void startTimer();
			void stopTimer();
			void abandonTimer();

			void TimerCountItemDisplay::cycleStarted()
			{
				running_time_ = 0;
				timer_ = new QTimer(this);
				timer_->setInterval(std::chrono::milliseconds(100));
				connect(timer_, &QTimer::timeout, this, &TimerCountItemDisplay::timerTick);
				timer_->start();
			}

			void TimerCountItemDisplay::cycleCompleted()
			{
					// We stopped
					timer_->stop();
					delete timer_;
					timer_ = nullptr;

					completed_count_++;
					completed_time_ += running_time_;
					updateDisplay();
			}

			void TimerCountItemDisplay::cycleAbandoned()
			{
				// We stopped
				timer_->stop();
				delete timer_;
				timer_ = nullptr;

				abandoned_count_++;
				abandoned_time_ += running_time_;
				updateDisplay();
			}

			void TimerCountItemDisplay::timerTick()
			{
				running_time_ += 0.1;
				updateDisplay();
			}

			void TimerCountItemDisplay::updateDisplay()
			{
				if (timer_ == nullptr)
				{
					running_time_label_->hide();
				}
				else
				{
					running_time_label_->show();
					running_time_label_->setText(QString::number(running_time_, 'f', 1));
				}

				completed_time_label_->setText("TimeC: " + QString::number(completed_time_, 'f', 1));
				completed_count_label_->setText("TotalC:" + QString::number(completed_count_));
				abandoned_time_label_->setText("TimeA: " + QString::number(abandoned_time_, 'f', 1));
				abandoned_count_label_->setText("TotalA:" + QString::number(abandoned_count_));
			}

			void TimerCountItemDisplay::setValues(const DataCollection& data)
			{
				assert(data.count() == 4);

				running_time_ = 0.0;
				left_->setStopped();
				for (int i = 0; i < data.count(); i++)
				{
					if (data.tag(i) == FormItemDesc::genComplexName(desc()->tag(), TimerCountFormItem::CompletedTimeField))
					{
						completed_time_ = data.data(i).toDouble();
					}
					else if (data.tag(i) == FormItemDesc::genComplexName(desc()->tag(), TimerCountFormItem::CompletedCountField))
					{
						completed_count_ = data.data(i).toInt();
					}
					else if (data.tag(i) == FormItemDesc::genComplexName(desc()->tag(), TimerCountFormItem::AbandonedTimeField))
					{
						abandoned_time_ = data.data(i).toDouble();
					}
					else if (data.tag(i) == FormItemDesc::genComplexName(desc()->tag(), TimerCountFormItem::AbandonedCountField))
					{
						abandoned_count_ = data.data(i).toInt();
					}
					else
					{
						assert(false);
					}
				}

				updateDisplay();
			}

			DataCollection TimerCountItemDisplay::getValues()
			{
				DataCollection d;

				d.add(FormItemDesc::genComplexName(desc()->tag(), TimerCountFormItem::CompletedTimeField), QVariant(completed_time_));
				d.add(FormItemDesc::genComplexName(desc()->tag(), TimerCountFormItem::CompletedCountField), QVariant(completed_count_));
				d.add(FormItemDesc::genComplexName(desc()->tag(), TimerCountFormItem::AbandonedTimeField), QVariant(abandoned_time_));
				d.add(FormItemDesc::genComplexName(desc()->tag(), TimerCountFormItem::AbandonedCountField), QVariant(abandoned_count_));

				return d;
			}
		}
	}
}
