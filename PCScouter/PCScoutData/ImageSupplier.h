#pragma once

#include <QString>
#include <QImage>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class ImageSupplier
			{
			public:
				virtual std::shared_ptr<QImage> get(const QString& tag) = 0;
			};
		}
	}
}
