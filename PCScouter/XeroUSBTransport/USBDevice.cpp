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

#include <Windows.h>
#include "USBDevice.h"
#include <stdio.h>
#include <iostream>

namespace xero
{
    namespace device
    {
        namespace usb
        {
            USBDevice::USBDevice(int vid, int pid, int ba, int da)
            {
                pid_ = pid;
                vid_ = vid;
                ba_ = ba;
                da_ = da;
                handle_ = NULL;
                memset(&api_, 0, sizeof(api_));

                buffer_.resize(512);
            }

            USBDevice::~USBDevice()
            {
                if (handle_ != NULL)
                {
                    UsbK_ReleaseInterface(handle_, 0, TRUE);
                    api_.Free(handle_);
                }
            }

            bool USBDevice::init()
            {
                KLST_HANDLE lh;
                KLST_DEVINFO_HANDLE dh;

                if (!LstK_Init(&lh, KLST_FLAG_NONE))
                    return false;

                while (true)
                {
                    if (!LstK_MoveNext(lh, &dh))
                        break;

                    if (dh->Common.Vid == vid_ && dh->Common.Pid == pid_)
                    {
                        if (!LibK_LoadDriverAPI(&api_, dh->DriverID))
                            continue;

                        if (!api_.Init(&handle_, dh))
                            continue;

                        if (!UsbK_ClaimInterface(handle_, 0, TRUE))
                        {
                            api_.Free(handle_);
                            continue;
                        }

                        desc_ = dh->DeviceDesc;
                        desc_ += ", vid " + std::to_string(dh->Common.Vid);
                        desc_ += ", pid " + std::to_string(dh->Common.Pid);
                        desc_ += ", bus " + std::to_string(dh->BusNumber);
                        desc_ += ", address " + std::to_string(dh->DeviceAddress);

                        LstK_Free(lh);
                        return true;
                    }
                }

                LstK_Free(lh);
                return false;
            }

            bool USBDevice::setTimeout(uint8_t pipe, int ms)
            {
                UINT tms = ms;
                if (!UsbK_SetPipePolicy(handle_, pipe | 0x80, PIPE_TRANSFER_TIMEOUT, (UINT)(sizeof(tms)), &tms))
                    return false;

                return true;
            }

            bool USBDevice::setShortPacketTerminate(uint8_t pipe, bool value)
            {
                UCHAR term = value;

                if (!UsbK_SetPipePolicy(handle_, pipe, SHORT_PACKET_TERMINATE, (UINT)(sizeof(term)), &term))
                    return false;

                return true;
            }

            bool USBDevice::setAutoClearStall(uint8_t pipe, bool value)
            {
                UCHAR term = value;

                if (!UsbK_SetPipePolicy(handle_, pipe, SHORT_PACKET_TERMINATE, (UINT)(sizeof(term)), &term))
                    return false;

                return true;
            }

            bool USBDevice::resetPipe(uint8_t pipe)
            {
                if (!UsbK_ResetPipe(handle_, pipe))
                    return false;

                return true;
            }

            bool USBDevice::abortPipe(uint8_t pipe)
            {
                if (!UsbK_AbortPipe(handle_, pipe))
                    return false;

                return true;
            }

            bool USBDevice::write(uint8_t pipe, const std::vector<uint8_t>& data)
            {
                UINT xfer;

                if (!UsbK_WritePipe(handle_, pipe, (PUCHAR)&data[0], (UINT)data.size(), &xfer, NULL)) {
                    std::cerr << "write failed" << std::endl;
                    return false;
                }

                if (xfer != data.size())
                    return false;

                std::cout << "wrote patcket length " << xfer << " error " << GetLastError() << std::endl;
                return true;
            }

            bool USBDevice::read(uint8_t pipe, std::vector<uint8_t>& data, size_t len)
            {
                UINT xfer;

                if (len == 0)
                    len = buffer_.size();

                if (!UsbK_ReadPipe(handle_, pipe, (PUCHAR)&buffer_[0], (UINT)len, &xfer, NULL))
                {
                    DWORD err = GetLastError();
                    if (err != ERROR_SEM_TIMEOUT)
                        return false;
                }

                data.resize(xfer);
                if (data.size() > 0)
                    memcpy(&data[0], &buffer_[0], xfer);

                return true;
            }
        }
    }
}
