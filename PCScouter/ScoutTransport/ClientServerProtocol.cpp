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

#include "ClientServerProtocol.h"
#include "CRCCCITT.h"

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			ClientServerProtocol::ClientServerProtocol(ScoutTransport* transport, bool server, bool freetrans, bool debug)
			{
				transport_ = transport;
				server_ = server;
				free_trans_ = freetrans;

				type_ = 0xFFFFFFFF;
				length_ = 0;
				debug_ = debug;

				QString msg;
				if (server)
					msg = "SERVER: ";
				else
					msg = "CLIENT: ";

				msg += transport->description();
				emit displayLogMessage(msg);
			}

			ClientServerProtocol::~ClientServerProtocol()
			{
				shutdown();
			}

			void ClientServerProtocol::start()
			{
				dataReceived_ = connect(transport_, &ScoutTransport::transportDataAvailable, this, &ClientServerProtocol::transportDataReceived);
				streamDisconnected_ = connect(transport_, &ScoutTransport::transportDisconnected, this, &ClientServerProtocol::transportDisconnected);
				errorReceived_ = connect(transport_, &ScoutTransport::transportError, this, &ClientServerProtocol::transportError);

				transportDataReceived();
			}

			QString ClientServerProtocol::pktTypeToString(uint32_t num)
			{
				QString ret;

				switch (num)
				{
				case TabletIDPacket:
					ret = "TabletIDPacket";
					break;

				case DataModelCorePacket:
					ret = "DataModelCorePacket";
					break;

				case DataModelScoutingPacket:
					ret = "DataModelScoutingPacket";
					break;

				case ScoutingDataReply:
					ret = "ScoutingDataReply";
					break;

				case TabletSelectionList:
					ret = "TabletSelectionList";
					break;

				case RequestScoutingData:
					ret = "RequestScoutingData";
					break;

				case RequestImageData:
					ret = "RequestScoutingData";
					break;

				case ProvideImageData:
					ret = "ProvideImageData";
					break;

				case ServerIDPacket:
					ret = "ServerIDPacket";
					break;

				case CoachIDPacket:
					ret = "CoachIDPacket";
					break;

				case RequestZebraData:
					ret = "RequestZebraData";
					break;

				case RequestMatchDetailData:
					ret = "RequestMatchDetailData";
					break;

				case CompleteButListening:
					ret = "CompleteButListening";
					break;

				case ProvideZebraData:
					ret = "ProvideZebraData";
					break;

				case ProvideMatchDetailData:
					ret = "ProvideMatchDetailData";
					break;

				case SyncDone:
					ret = "SyncDone";
					break;

				case ErrorReply:
					ret = "ErrorReply";
					break;

				default:
					ret = "UnknownPacketType";
					break;
				}

				return ret;
			}

			void ClientServerProtocol::run()
			{
				if (transport_ != nullptr)
					transport_->run();
			}

			void ClientServerProtocol::transportDisconnected()
			{
				shutdown();
				emit disconnected();
			}

			void ClientServerProtocol::transportError(const QString& errmsg)
			{
				shutdown();
				emit errorDetected(errmsg);
			}

			void ClientServerProtocol::shutdown()
			{
				if (transport_ != nullptr)
				{
					transport_->flush();
					disconnect(dataReceived_);
					disconnect(streamDisconnected_);
					disconnect(errorReceived_);

					transport_->close();
					delete transport_;
					transport_ = nullptr;
				}
			}

			void ClientServerProtocol::logJsonPacket(const char* oper, uint32_t pkt, const QJsonDocument& doc)
			{
				QString msg = (server_ ? "Server:" : "Client:");
				msg += oper;
				msg += ": packet " + pktTypeToString(pkt);
				QString str = doc.toJson(QJsonDocument::JsonFormat::Compact);
				msg += "[" + QString::number(str.length()) + "] ";
				if (str.length() > 80)
					str = str.left(76) + " ...";

				msg += ", " + str;

				if (debug_)
					emit displayLogMessage(msg);
			}

			void ClientServerProtocol::sendJson(uint32_t ptype, const QJsonDocument& doc, int comptype)
			{
				if (transport_ == nullptr)
				{
					QString msg = (server_ ? "Server:" : "Client:");
					msg += "sendJson: cannot send JSON message, transport_ is nullptr";
					emit displayLogMessage(msg);
					return;
				}

				logJsonPacket("sendJson", ptype, doc);

				QByteArray data = doc.toJson();

				switch (comptype)
				{
				case 0:
					// Uncompressed, to nothing
					break;

				case 1:
					data = qCompress(data);
					break;

				default:
					QString msg = (server_ ? "Server:" : "Client:");
					msg += "sendJson: cannot send JSON message, invalid compression type - ";
					msg += QString::number(comptype);
					emit displayLogMessage(msg);
					return;
					break;
				}

				uint32_t len = data.size();

				data.push_front((comptype) & 0xFF);

				data.push_front((len >> 24) & 0xFF);
				data.push_front((len >> 16) & 0xFF);
				data.push_front((len >> 8) & 0xFF);
				data.push_front((len >> 0) & 0xFF);

				data.push_front((ptype >> 24) & 0xFF);
				data.push_front((ptype >> 16) & 0xFF);
				data.push_front((ptype >> 8) & 0xFF);
				data.push_front((ptype >> 0) & 0xFF);

				quint16 sum = CRCCCITT::checksum(data);

				quint8 bl = sum & 0xff;
				quint8 bh = (sum >> 8) & 0xFF;

				data.push_back(bl);
				data.push_back(bh);

				transport_->write(data);
			}

			void ClientServerProtocol::transportDataReceived()
			{
				QByteArray data = transport_->readAll();
				data_.append(data);

				emit displayLogMessage("read " + QString::number(data.size()) + " bytes from transport '" + transport_->type() + "'");

				// Reading a new packet
				if (data_.size() < 8)
					return;

				uint8_t b0 = data_[4];
				uint8_t b1 = data_[5];
				uint8_t b2 = data_[6];
				uint8_t b3 = data_[7];

				length_ = static_cast<uint32_t>(b0 | (b1 << 8) | (b2 << 16) | (b3 << 24));

				// See if we have the complete packet, if not wait for more data
				int total = length_ + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t);
				if (data_.size() < total)
					return;

				// Now we have an entire packet
				quint16 calcsum = CRCCCITT::checksum(data_, 0, total - sizeof(uint16_t));

				quint8 bl = data_[total - 2];
				quint8 bh = data_[total - 1];
				quint16 recvsum = (bl << 0) | (bh << 8);

				recvsum = calcsum;
				if (calcsum != recvsum) {
					//
					// Checksum error, fail
					//
					shutdown();
					emit errorDetected("checksum error in received data");
					return;
				}

				b0 = data_[0];
				b1 = data_[1];
				b2 = data_[2];
				b3 = data_[3];

				int comp_type = data_[8];

				int type = static_cast<uint32_t>(b0 | (b1 << 8) | (b2 << 16) | (b3 << 24));
				data_ = data_.remove(0, 9);
				data.resize(length_);
				memcpy(data.data(), data_.data(), length_);
				data_ = data_.remove(0, length_ + 2);

				switch (comp_type) {
				case 0:
					// Not compressed - do nothing
					break;

				case 1:
					// ZLib compression
					data = qUncompress(data);
					break;

				default:
					shutdown();
					emit errorDetected("packet detected with invalid compression type - " + QString::number(comp_type));
					return;
					break;
				}

				QJsonDocument doc = QJsonDocument::fromJson(data);
				if (doc.isNull()) {
					doc = QJsonDocument::fromJson(data);
					if (doc.isNull())
					{
						shutdown();
						emit errorDetected("invalid JSON document received in packet");
					}
				}

				logJsonPacket("jsonReceived", type, doc);
				emit jsonReceived(type, doc);
			}
		}
	}
}
