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

#pragma once
#include "XeroUSBTransport.h"
#include "Windows.h"
#include <vector>
#include <memory>
#include <string>
#include <stdint.h>
#include <sstream>

namespace xero
{
	namespace device
	{
		namespace usb
		{
			class XEROUSBTRANSPORT_API XeroPCCableTransfer
			{
			public:
				XeroPCCableTransfer(int ba = -1, int da = -1);
				virtual ~XeroPCCableTransfer();

				void setIDs(uint16_t vid, uint16_t pid) {
					vid_ = vid;
					pid_ = pid;
				}

				bool init(std::stringstream &messages);

				bool send(const std::vector<uint8_t>& data);
				bool receive(std::vector<uint8_t>& data);

				bool reset();

				const std::string& description();

			private:
				static constexpr const uint16_t default_vid = 0x67B;
				static constexpr const uint16_t default_pid = 0x25A1;
				static constexpr const uint8_t sendPipe = 0x02;
				static constexpr const uint8_t recvPipe = 0x83;

			private:
				int pid_;
				int vid_;
				int ba_;
				int da_;
				void* dev_;
			};
		}
	}
}
