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
				active_child_ = nullptr;
				inited_ = false;
				connect_signaled_ = false;
			}

			USBServer::~USBServer()
			{
				if (active_child_ != nullptr)
				{
					delete active_child_;
					active_child_ = nullptr;
				}
			}

			QString USBServer::hwinfo()
			{
				QString ret = "USBServer - no USB hardware open";

				if (active_child_ != nullptr)
					ret = active_child_->description();

				return ret;
			}

			bool USBServer::init()
			{
				active_child_ = new USBTransport(this);
				return active_child_->init();
			}

			void USBServer::closing(USBTransport* trans)
			{
				assert(trans == active_child_);

				delete active_child_;

				active_child_ = new USBTransport(this);
				active_child_->init();
				connect_signaled_ = false;
			}

			void USBServer::run()
			{
				if (active_child_ != nullptr)
				{
					active_child_->run();
					if (active_child_->hasData() && connect_signaled_ == false)
					{
						emit connected(active_child_);
						connect_signaled_ = true;
					}
				}
			}
		}
	}
}
