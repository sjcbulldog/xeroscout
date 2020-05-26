#pragma once

#include <QString>
#include <QStringList>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ImageItemPublishInfo
			{
			public:
				enum class PublishType
				{
					Average,
					Sum,
				};

			public:
				ImageItemPublishInfo(const QString& tag, PublishType type) {
					tag_ = tag;
					type_ = type;
				}

				void addComponents(const QString& comp) {
					components_.push_back(comp);
				}

				const QStringList& components() const {
					return components_;
				}

				PublishType type() const {
					return type_;
				}

				const QString& tag() const {
					return tag_;
				}

			private:
				QString tag_;
				QStringList components_;
				PublishType type_;
			};
		}
	}
}