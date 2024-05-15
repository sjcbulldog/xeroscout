#include "BLEServer.h"
#include <QtCore/QList>
#include <QtBluetooth/QLowEnergyAdvertisingData>
#include <QtBluetooth/QLowEnergyAdvertisingParameters>
#include <QtBluetooth/QLowEnergyCharacteristicData>
#include <QtBluetooth/QLowEnergyDescriptorData>

namespace xero
{
	namespace scouting
	{
		namespace transport {

			BLEServer::BLEServer(QObject* parent) : ScoutServer(parent)
			{
				ctrl_ = nullptr;
				service_ = nullptr;
			}

			BLEServer::~BLEServer()
			{
			}

			void BLEServer::run()
			{
			}

			bool BLEServer::createService()
			{
				QByteArray data;
				data.fill(0, 20);

				QLowEnergyCharacteristicData cdata;
				cdata.setUuid(QBluetoothUuid(SyncCharUUID));
				cdata.setValue(data);
				cdata.setValueLength(0, data.size());
				cdata.setProperties(QLowEnergyCharacteristic::Notify);

				QLowEnergyDescriptorData clientConfig(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration, QByteArray(20, 0));
				cdata.addDescriptor(clientConfig);
				
				data_.setType(QLowEnergyServiceData::ServiceTypePrimary);
				data_.setUuid(QBluetoothUuid(SyncServiceUUID));
				data_.addCharacteristic(cdata);

				return true;
			}

			bool BLEServer::init(std::stringstream& messages)
			{
				qDebug() << "Starting init";

				if (!createService())
					return false;

				QLowEnergyAdvertisingData data;
				QList<QBluetoothUuid> services;
				QBluetoothUuid uid(SyncServiceUUID);

				services << uid;

				data.setDiscoverability(QLowEnergyAdvertisingData::Discoverability::DiscoverabilityGeneral);
				data.setLocalName("XeroServer");
				data.setServices(services);

				ctrl_ = QLowEnergyController::createPeripheral();
				(void)connect(ctrl_, &QLowEnergyController::errorOccurred, this, &BLEServer::errorOccurred);
				
				service_ = ctrl_->addService(data_);

				ctrl_->startAdvertising(QLowEnergyAdvertisingParameters(), data, data);

				return true;
			}

			void BLEServer::errorOccurred(QLowEnergyController::Error code)
			{

			}

			QString BLEServer::hwinfo()
			{
				return "BLE";
			}
		}
	}
}
