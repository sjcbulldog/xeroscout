#pragma once

#include <QString>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ZebraEvent
			{
			public:
				enum class EventType
				{
					Enter,
					Leave,
					Idle
				};

			public:
				ZebraEvent(const QString& name, double t, EventType etype) {
					name_ = name;
					time_ = t;
					etype_  = etype ;
				}

				ZebraEvent(double duration, double t) {
					time_ = t;
					etype_ = EventType::Idle;
					duration_ = duration;
				}

				const QString& name() const {
					return name_;
				}

				bool isIdle() const {
					return etype_ == EventType::Idle;
				}

				bool isEnter() const {
					return etype_ == EventType::Enter;
				}

				bool isLeave() const {
					return etype_ == EventType::Leave;
				}

				double when() const {
					return time_;
				}

				void setDuration(double d) {
					duration_ = d;
				}

				double duration() const {
					return duration_;
				}

				QString toString() const {
					QString ret = "[";

					if (etype_ == EventType::Idle)
					{
						ret += "idle ";
						ret += QString::number(duration_, 'f', 2) + "@" + QString::number(time_, 'f', 2) + "]";
					}
					else
					{
						if (etype_ == EventType::Enter)
							ret += "entered ";
						else if (etype_ == EventType::Leave)
							ret += "left ";

						ret += name_ + "@" + QString::number(time_, 'f', 2) + "]";
					}

					return ret;
				}

			private:
				double time_;
				QString name_;
				EventType etype_;
				double duration_;
			};
		}
	}
}
