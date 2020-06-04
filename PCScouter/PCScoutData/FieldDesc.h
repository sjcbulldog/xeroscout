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
				FieldDesc(const QString &name, FieldDesc::Type type, bool editable, bool temp = false) {
					name_ = name;
					type_ = type;
					temporary_ = temp;
					editable_ = editable;
					warning_limits_ = false;
				}

				FieldDesc(const QString &name, const QStringList& list, bool editable, bool temp = false) {
					name_ = name;
					choices_ = list;
					type_ = Type::StringChoice;
					temporary_ = temp;
					editable_ = editable;
					warning_limits_ = false;
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

				bool isEditable() const {
					return editable_;
				}

				void setLimits(double minv, double maxv) {
					min_limit_ = minv;
					max_limit_ = maxv;
					warning_limits_ = true;
				}

				double hasLimits() const {
					return warning_limits_;
				}

				double minLimit() const {
					return min_limit_;
				}

				double maxLimit() const {
					return max_limit_;
				}

			private:
				QString name_;
				Type type_;
				QStringList choices_;
				bool temporary_;
				bool editable_;
				bool warning_limits_;
				double min_limit_;
				double max_limit_;
			};
		}
	}
}
