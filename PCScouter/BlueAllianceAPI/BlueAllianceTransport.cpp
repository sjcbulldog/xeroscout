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

#include "BlueAllianceTransport.h"
#include "BACountResult.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QTextCodec>
#include <QAuthenticator>
#include <QDebug>
#include <thread>

namespace xero
{
	namespace ba
	{


		BlueAllianceTransport::BlueAllianceTransport(const QString& server, const QString& authkey)
		{
			server_ = server;
			authkey_ = authkey;
			reply_ = nullptr;
			netmgr_ = new QNetworkAccessManager(this);

			(void)connect(netmgr_, &QNetworkAccessManager::finished, this, &BlueAllianceTransport::finished);
			(void)connect(netmgr_, &QNetworkAccessManager::sslErrors, this, &BlueAllianceTransport::sslError);
			(void)connect(netmgr_, &QNetworkAccessManager::authenticationRequired, this, &BlueAllianceTransport::authRequired);
		}

		BlueAllianceTransport::~BlueAllianceTransport()
		{
		}

		void BlueAllianceTransport::authRequired(QNetworkReply* reply, QAuthenticator* auth)
		{
		}

		bool BlueAllianceTransport::request(QString req, std::function<void(std::shared_ptr<QJsonDocument>, BlueAllianceResult::Status st, int code)> cb)
		{
			//
			// Store the callback
			//
			cb_ = cb;

			//
			// Create the network request for the blue alliance server
			//
			QUrl url(server_ + req);
			QNetworkRequest netreq(QUrl(server_ + req));
			netreq.setHeader(QNetworkRequest::UserAgentHeader, "XeroScouter");
			netreq.setRawHeader(AuthKeyHeaderName, authkey_.toUtf8());

			//
			// Send the request to the server
			//
			reply_ = netmgr_->get(netreq);
			auto err = reply_->error();
			if (err != QNetworkReply::NetworkError::NoError)
			{
				//
				// Request failed right away, schedule an error packet as a result
				//
				reply_ = nullptr;
				return false;
			}

			return true;
		}

		void BlueAllianceTransport::finished()
		{
			if (reply_ == nullptr)
				return;

			std::shared_ptr<QJsonDocument> doc;
			BlueAllianceResult::Status st = BlueAllianceResult::Status::Success;

			QNetworkReply::NetworkError err = reply_->error();
			if (err != QNetworkReply::NetworkError::NoError)
			{
				doc = nullptr;
				st = BlueAllianceResult::Status::ConnectionError;
			}
			else
			{
				//
				// Get the data from the reply
				//
				QByteArray bdata = reply_->readAll();

				//
				// Now, convert the result to a JSON document and hand to the post processor
				//
				QJsonParseError err;
				doc = std::make_shared<QJsonDocument>();
				*doc = QJsonDocument::fromJson(bdata, &err);
				if (err.error != QJsonParseError::NoError)
				{
					doc = nullptr;
					st = BlueAllianceResult::Status::JSONError;
				}
			}

			int code = reply_->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
			reply_ = nullptr;
			if (cb_ != nullptr)
			{
				cb_(doc, st, code);
			}
		}

		void BlueAllianceTransport::sslError(QNetworkReply* reply, const QList<QSslError>& errors)
		{
			delete reply_;
			reply_ = nullptr;
			cb_(nullptr, BlueAllianceResult::Status::SSLError, -1);
		}

	}
}
