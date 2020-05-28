#pragma once

#include <QStringList>
#include <QString>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class FieldDesc
			{
			public:
				enum class Type
				{
					Integer,
					Double,
					Boolean,
					StringChoice,
					String
				};

			public:
				FieldDesc(const QString &name, FieldDesc::Type type) {
					name_ = name;
					type_ = type;
				}

				FieldDesc(const QString &name, const QStringList& list) {
					name_ = name;
					choices_ = list;
					type_ = Type::StringChoice;
				}

				FieldDesc::Type type() const {
					return type_;
				}

				const QString& name() const {
					return name_;
				}

				void addChoice(const QString& choice) {
					choices_.push_back(choice);
				}

			private:
				QString name_;
				Type type_;
				QStringList choices_;
			};
		}
	}
}
