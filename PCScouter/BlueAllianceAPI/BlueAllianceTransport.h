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

#include "BlueAllianceResult.h"
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QUrl>
#include <mutex>
#include <functional>
#include <memory>

namespace xero
{
	namespace ba
	{


		class BlueAllianceTransport : public QObject
		{
			Q_OBJECT;

		public:
			BlueAllianceTransport(const QString& server, const QString& authkey);
			virtual ~BlueAllianceTransport();

			bool request(QString req, std::function<void(std::shared_ptr<QJsonDocument> doc, BlueAllianceResult::Status st, int code)> cb);

		private:
			static constexpr const char* AuthKeyHeaderName = "X-TBA-Auth-Key";

		private:
			void readReplyData();
			void networkError(QNetworkReply::NetworkError code);
			void sslError(QNetworkReply* reply, const QList<QSslError>& errors);
			void finished();
			void authRequired(QNetworkReply* reply, QAuthenticator* auth);

		private:
			QString server_;
			QString authkey_;

			QNetworkAccessManager* netmgr_;
			QNetworkReply* reply_;
			std::function<void(std::shared_ptr<QJsonDocument>, BlueAllianceResult::Status st, int)> cb_;
		};

	}
}
