#pragma once

#include "ScoutTransport.h"
#include <QBluetoothUuid>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			class BluetoothIDS
			{
			private:
				BluetoothIDS();
				virtual ~BluetoothIDS();

			public:
				static const QBluetoothUuid &serviceID() {
					return service_id_;
				}

				static QString serviceName(int team) {
					return service_name_ + "-" + QString::number(team);
				}

			private:
				static const QBluetoothUuid service_id_;
				static const QString service_name_;
			};
		}
	}
}


