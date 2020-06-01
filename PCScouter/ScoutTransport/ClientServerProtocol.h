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

#include "scouttransport_global.h"
#include "ScoutTransport.h"
#include <QDebug>
#include <QJsonDocument>

namespace xero
{
	namespace scouting
	{
		namespace transport
		{
			class SCOUTTRANSPORT_EXPORT ClientServerProtocol : public QObject
			{
				Q_OBJECT

			public:
				static constexpr const quint16 ScoutBroadcastPort = 45454;
				static constexpr const quint16 ScoutStreamPort = 45455;

				static constexpr const uint32_t TabletIDPacket = 0x80000001;
				static constexpr const uint32_t DataModelCorePacket = 0x80000002;
				static constexpr const uint32_t DataModelScoutingPacket = 0x80000003;
				static constexpr const uint32_t ScoutingDataReply = 0x80000004;
				static constexpr const uint32_t TabletSelectionList = 0x80000005;
				static constexpr const uint32_t RequestScoutingData = 0x80000006;
				static constexpr const uint32_t RequestImageData = 0x80000007;
				static constexpr const uint32_t ProvideImageData = 0x80000008;
				static constexpr const uint32_t ServerIDPacket = 0x80000009;
				static constexpr const uint32_t SyncDone = 0x80001000;
				static constexpr const uint32_t ErrorReply = 0x8000FFFF;

			public:
				ClientServerProtocol(ScoutTransport* transport, bool server, bool freetrans, bool debug);
				virtual ~ClientServerProtocol();

				void sendJson(uint32_t ptype, const QJsonDocument& doc, int comptype);
				void shutdown();

				void run();
				void start();

				QString transportType() { return transport_->type(); }

				static QString pktTypeToString(uint32_t type);

			signals:
				void jsonReceived(uint32_t ptype, const QJsonDocument& doc);
				void errorDetected(const QString& errmsg);
				void disconnected();
				void displayLogMessage(const QString& msg);

			private:
				void transportDataReceived();
				void transportDisconnected();
				void transportError(const QString& errmsg);
				void logJsonPacket(const char* oper, uint32_t pkt, const QJsonDocument& doc);

			private:
				uint32_t type_;
				uint32_t length_;
				ScoutTransport* transport_;

				QByteArray data_;
				bool debug_;
				bool server_;
				bool free_trans_;

				QMetaObject::Connection dataReceived_;
				QMetaObject::Connection streamDisconnected_;
				QMetaObject::Connection errorReceived_;
			};

		}
	}
}
