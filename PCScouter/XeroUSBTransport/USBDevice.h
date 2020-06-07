//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
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
				bool write(uint8_t pipe, const std::vector<uint8_t>& data);
				bool read(uint8_t pipe, std::vector<uint8_t>& data, size_t len = 0);
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

				std::vector<uint8_t> buffer_;

				std::string desc_;
			};
		}
	}
}
