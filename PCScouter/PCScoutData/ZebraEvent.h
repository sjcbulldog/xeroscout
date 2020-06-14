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
				ZebraEvent(const QString& name, double t, bool enter) {
					name_ = name;
					time_ = t;
					enter_ = enter_;
				}

				const QString& name() const {
					return name_;
				}

				bool isEnter() const {
					return enter_;
				}

				bool isExit() const {
					return !enter_;
				}

				double when() const {
					return time_;
				}

			private:
				double time_;
				QString name_;
				bool enter_;
			};
		}
	}
}
