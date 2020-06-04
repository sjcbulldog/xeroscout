//
// Copyright 2020 by Jack W. (Butch) Griffin
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
// 

#include "pch.h"
#include "XeroPCCableTransfer.h"
#include "USBDevice.h"
#include <iostream>

namespace xero
{
	namespace device
	{
		namespace usb
		{
			XeroPCCableTransfer::XeroPCCableTransfer(int ba, int da)
			{
				vid_ = default_vid;
				pid_ = default_pid;
				da_ = da;
				ba_ = ba;
				dev_ = nullptr;
			}

			XeroPCCableTransfer::~XeroPCCableTransfer()
			{
				USBDevice* dev = reinterpret_cast<USBDevice*>(dev_);
				delete dev;
			}

			bool XeroPCCableTransfer::init()
			{
				USBDevice* dev = new USBDevice(vid_, pid_, ba_, da_);
				if (!dev->init())
				{
					delete dev;
					dev_ = nullptr;
					return false;
				}

				if (!dev->resetPipe(recvPipe))
				{
					delete dev;
					return false;
				}

				if (!dev->resetPipe(sendPipe))
				{
					delete dev;
					return false;
				}

				if (!dev->abortPipe(recvPipe))
				{
					delete dev;
					return false;
				}

				if (!dev->abortPipe(sendPipe))
				{
					delete dev;
					return false;
				}

				if (!dev->setTimeout(recvPipe, 1))
				{
					delete dev;
					return false;
				}

				if (!dev->setShortPacketTerminate(sendPipe, true))
				{
					delete dev;
					return false;
				}

				if (!dev->setAutoClearStall(sendPipe, true))
				{
					delete dev;
					return false;
				}

				if (!dev->setAutoClearStall(recvPipe, true))
				{
					delete dev;
					return false;
				}

				dev_ = dev;
				return true;
			}

			bool XeroPCCableTransfer::send(const std::vector<uint8_t>& data)
			{
				USBDevice* dev = reinterpret_cast<USBDevice*>(dev_);
				return dev->write(sendPipe, data);
			}

			bool XeroPCCableTransfer::receive(std::vector<uint8_t>& data)
			{
				USBDevice* dev = reinterpret_cast<USBDevice*>(dev_);
				return dev->read(recvPipe, data);
			}


			const std::string& XeroPCCableTransfer::description()
			{
				static std::string invalid_("invalid") ;

				USBDevice* dev = reinterpret_cast<USBDevice*>(dev_);
				if (dev == nullptr)
					return invalid_;

				return dev->description();
			}
		}
	}
}
