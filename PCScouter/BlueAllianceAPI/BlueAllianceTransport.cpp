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
			Q_UNUSED(reply);
			Q_UNUSED(auth);
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
			QString result;

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
				result = QString::fromUtf8(bdata);

				//
				// Now, convert the result to a JSON document and hand to the post processor
				//
				QJsonParseError perr;
				doc = std::make_shared<QJsonDocument>();
				*doc = QJsonDocument::fromJson(bdata, &perr);
				if (perr.error != QJsonParseError::NoError)
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
			Q_UNUSED(errors);
			Q_UNUSED(reply);

			delete reply_;
			reply_ = nullptr;
			cb_(nullptr, BlueAllianceResult::Status::SSLError, -1);
		}

	}
}
