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
