//
// Copyright(C) 2020 by Jack (Butch) Griffin
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see < https://www.gnu.org/licenses/>.
//
//
//
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
//

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
