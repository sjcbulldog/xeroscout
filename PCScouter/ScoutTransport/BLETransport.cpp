#include "BLETransport.h"

namespace xero
{
	namespace scouting
	{
		namespace transport
		{

			BLETransport::BLETransport(bool server) : ScoutTransport(server)
			{

			}

			BLETransport::~BLETransport()
			{

			}

			bool BLETransport::init(std::stringstream& messages)
			{
				return true;
			}

			bool BLETransport::write(const QByteArray& data)\
			{
				return true;
			}

			QByteArray BLETransport::readAll()
			{
				return QByteArray();
			}
			void BLETransport::flush()
			{

			}

			void BLETransport::run()
			{

			}

			QString BLETransport::type()
			{
				return "BLE";
			}

			QString BLETransport::description()
			{
				return "";
			}

			void BLETransport::close()
			{

			}
		}
	}
}
