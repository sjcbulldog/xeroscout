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


#include <iostream>
#include "XeroPCCableTransfer.h"
#include <vector>
#include <stdint.h>

std::vector<uint8_t> data = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
std::vector<uint8_t> recdata;

int main()
{
	xero::device::usb::XeroPCCableTransfer dev1(0, 1);
	xero::device::usb::XeroPCCableTransfer dev2(0, 4);

	if (!dev1.init())
		return 1;

	if (!dev2.init())
		return 1;

	dev1.send(data);

	int i = 0;
	while (true)
	{
		dev2.receive(recdata);
		std::cout << "received " << recdata.size() << " bytes" << std::endl;
	}
}
