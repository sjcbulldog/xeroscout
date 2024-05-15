#pragma once

#include "ScoutServer.h"
#include <QtBluetooth/QLowEnergyController>
#include <QtBluetooth/QLowEnergyServiceData>

namespace xero
{
	namespace scouting
	{
		namespace transport {

			class SCOUTTRANSPORT_EXPORT BLEServer : public ScoutServer
			{
				friend class BLETransport;

			public:
				static constexpr const char* SyncServiceUUID = "1d851fba-b170-4e90-8092-053d26ae6965";
				static constexpr const char* SyncCharUUID = "f81931cb-6849-4698-ac3f-f4208a349127";

			public:
				BLEServer(QObject* parent);
				virtual ~BLEServer();

				void run() override;
				bool init(std::stringstream& messages) override;
				QString name() override { return "BLE"; }
				virtual QString hwinfo();

			private:
				void errorOccurred(QLowEnergyController::Error code);
				bool createService();

			private:
				QLowEnergyController *ctrl_;
				QLowEnergyService* service_;
				QLowEnergyServiceData data_;
			};
		}
	}
}
