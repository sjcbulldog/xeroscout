#include "USBServer.h"
#include "USBTransport.h"
#include <cassert>

using namespace xero::device::usb;

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			USBServer::USBServer(QObject *parent) : ScoutServer(parent)
			{
				usb_ = new XeroPCCableTransfer(-1, -1);
				active_child_ = nullptr;
				future_child_ = nullptr;
				dead_child_ = nullptr;
			}

			USBServer::~USBServer()
			{
				if (active_child_ != nullptr)
				{
					delete active_child_;
					active_child_ = nullptr;
				}

				if (future_child_ != nullptr)
				{
					delete future_child_;
					future_child_ = nullptr;
				}

				if (usb_ != nullptr)
				{
					delete usb_;
					usb_ = nullptr;
				}
			}

			QString USBServer::hwinfo()
			{
				return QString(usb_->description().c_str());
			}

			bool USBServer::init()
			{
				active_child_ = nullptr;
				future_child_ = nullptr;
				dead_child_ = nullptr;

				if (!usb_->init())
					return false;

				future_child_ = new USBTransport(this, usb_);

				return true;
			}

			void USBServer::closing(USBTransport* trans)
			{
				assert(trans == active_child_);

				dead_child_ = active_child_;
				active_child_ = nullptr;
				future_child_ = new USBTransport(this, usb_);
			}

			void USBServer::run()
			{
				if (dead_child_ != nullptr)
				{
					dead_child_ = nullptr;
				}

				if (active_child_ != nullptr)
				{
					active_child_->run();
				}
				else if (future_child_ != nullptr)
				{
					future_child_->run();
					if (future_child_->hasData())
					{
						active_child_ = future_child_;
						future_child_ = nullptr;

						emit connected(active_child_);
					}
				}
			}
		}
	}
}
