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

#include <libusbk.h>
#include <vector>
#include <string>
#include <stdint.h>

namespace xero
{
	namespace device
	{
		namespace usb
		{
			class USBDevice
			{
			public:
				USBDevice(int vid, int pid, int ba = -1, int da = -1);
				virtual ~USBDevice();

				const std::string& description() const {
					return desc_;
				}

				bool init();
				bool write(uint8_t sendpipe, uint8_t recvpipe, const std::vector<uint8_t>& data);
				bool read(uint8_t sendpipe, uint8_t recvpipe, std::vector<uint8_t>& data, size_t len = 0);
				bool setTimeout(uint8_t pipe, int ms);
				bool setShortPacketTerminate(uint8_t pipe, bool value);
				bool setAutoClearStall(uint8_t pipe, bool value);
				bool resetPipe(uint8_t pipe);
				bool abortPipe(uint8_t pipe);

			private:
				int pid_;
				int vid_;
				int ba_;
				int da_;
				KUSB_DRIVER_API api_;
				KUSB_HANDLE handle_;

				std::vector<uint8_t> read_buffer_;
				std::vector<uint8_t> write_buffer_;

				std::string desc_;

				int packet_no_;
			};
		}
	}
}
