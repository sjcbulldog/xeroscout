#pragma once

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			enum class Alliance {
				Red,
				Blue
			};

			inline QString toString(Alliance c) {
				QString ret;

				if (c == Alliance::Red)
					ret = "red";
				else
					ret = "blue";

				return ret;
			}

			inline Alliance allianceFromString(const QString& str) {
				assert(str == "red" || str == "blue");

				if (str == "red")
					return Alliance::Red;

				return Alliance::Blue;
			}
		}
	}
}
