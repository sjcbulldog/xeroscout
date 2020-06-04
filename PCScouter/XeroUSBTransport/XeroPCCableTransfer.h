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

#pragma once
#include "XeroUSBTransport.h"
#include "Windows.h"
#include <vector>
#include <memory>
#include <string>
#include <stdint.h>

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

				bool init();

				bool send(const std::vector<uint8_t>& data);
				bool receive(std::vector<uint8_t>& data);

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
