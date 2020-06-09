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
				static constexpr const uint32_t CoachIDPacket = 0x8000000A;
				static constexpr const uint32_t RequestZebraData = 0x8000000B;
				static constexpr const uint32_t RequestMatchDetailData = 0x8000000C;
				static constexpr const uint32_t CompleteButListening = 0x8000000D;
				static constexpr const uint32_t ProvideZebraData = 0x8000000E;
				static constexpr const uint32_t ProvideMatchDetailData = 0x8000000F;
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
