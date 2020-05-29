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
				FieldDesc(const QString &name, FieldDesc::Type type, bool temp = false) {
					name_ = name;
					type_ = type;
					temporary_ = temp;
				}

				FieldDesc(const QString &name, const QStringList& list, bool temp = false) {
					name_ = name;
					choices_ = list;
					type_ = Type::StringChoice;
					temporary_ = temp;
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

				const QStringList& choices() const {
					return choices_;
				}

				bool isTemporary() const {
					return temporary_;
				}

			private:
				QString name_;
				Type type_;
				QStringList choices_;
				bool temporary_;
			};
		}
	}
}
