#include "ImportStatboticsController.h"
#include "DataModelBuilder.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

ImportStatboticsController::ImportStatboticsController(std::shared_ptr<xero::ba::BlueAlliance> ba, 
	std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm, int year) : ApplicationController(ba, dm)
{
	state_ = State::Start;
	year_ = year;

	netmgr_ = new QNetworkAccessManager(this);
	reply_ = nullptr;

	(void)connect(netmgr_, &QNetworkAccessManager::finished, this, &ImportStatboticsController::finished);
	(void)connect(netmgr_, &QNetworkAccessManager::sslErrors, this, &ImportStatboticsController::sslError);
}

ImportStatboticsController::~ImportStatboticsController()
{
}

int ImportStatboticsController::percentDone()
{
	int ret = 0;

	if (state_ == State::WaitingForData)
	{
		if (teams_.size() == 0) {
			ret = 100;
		}
		else {
			int completed = total_teams_ - teams_.size();
			double pcnt = (double)completed / (double)total_teams_;
			ret = (int)(pcnt * 100);
		}
	}

	return ret;
}

bool ImportStatboticsController::isDone()
{
	return state_ == State::Done || state_ == State::Error;
}

void ImportStatboticsController::requestNextTeam()
{
	current_team_ = teams_.front();
	teams_.pop_front();

	QString urlstr(Server);
	urlstr += "/team_year/" + QString::number(current_team_) + "/" + QString::number(year_);

	qDebug() << "connection to host '" << urlstr << "'";

	QUrl url(urlstr);
	QNetworkRequest netreq(urlstr);
	netreq.setHeader(QNetworkRequest::UserAgentHeader, "XeroScouter");

	reply_ = netmgr_->get(netreq);
	auto err = reply_->error();
	if (err != QNetworkReply::NetworkError::NoError)
	{
		reply_ = nullptr;
		state_ = State::Error;
	}
}

void ImportStatboticsController::finished()
{
	QNetworkReply::NetworkError err = reply_->error();
	if (err != QNetworkReply::NetworkError::NoError)
	{
		emit errorMessage("could not reach statbiotics host");
		state_ = State::Error;
		emit complete(false);
		return;
	}

	//
	// Get the data from the reply
	//
	QByteArray bdata = reply_->readAll();
	QString str = QString::fromUtf8(bdata).trimmed();
	QJsonDocument doc;
	if (str != "null") {
		//
		// Now, convert the result to a JSON document and hand to the post processor
		//
		QJsonParseError perr;
		doc = QJsonDocument::fromJson(bdata, &perr);
		if (perr.error != QJsonParseError::NoError)
		{
			state_ = State::Error;
			return;
		}
		else {
			QString tkey = "frc" + QString::number(current_team_);
			data_.insert(tkey, doc);
		}
	}

	if (teams_.count() == 0) {
		xero::scouting::datamodel::DataModelBuilder::addStatboticsData(dataModel(), data_);
		state_ = State::Done;
		emit complete(true);
	}
	else {
		requestNextTeam();
	}
}

void ImportStatboticsController::sslError(QNetworkReply* reply, const QList<QSslError>& errors)
{
}

void ImportStatboticsController::run()
{
	if (state_ == State::Start)
	{
		std::shared_ptr<xero::scouting::datamodel::ScoutingDataModel> dm = dataModel();
		for (auto team : dm->teams()) {
			teams_.push_back(team->number());
		}
		total_teams_ = teams_.count();

		requestNextTeam();
		state_ = State::WaitingForData;
	}
}