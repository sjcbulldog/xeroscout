#pragma once

#include "pcscoutdata_global.h"
#include <QObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QRandomGenerator>
#include <QVariant>
#include <map>
#include <list>
#include <random>
#include <memory>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class PCSCOUTDATA_EXPORT GameRandomProfile : public QObject
			{
				Q_OBJECT

			public:
				typedef std::list<std::pair<QString, double>> StringChoices;

			public:
				GameRandomProfile();
				virtual ~GameRandomProfile();

				void load(const QString& path);

				QVariant generateRandomBool(const QString& field);
				QVariant generateRandomChoice(const QString& field, const QStringList &choices);

				QVariant generateRandomInteger(const QString& field, int minv, int maxv);
				QVariant generateRandomText(const QString& field);

				QVariant generateRandomReal(const QString& field, double minv, double maxv);

			signals:
				void error(const QString& str);

			private:
				bool isArrayDouble(const QJsonArray& array);
				bool isArrayString(const QJsonArray& array);
				void generateDriveBase(const QStringList &types);

			private:
				QRandomGenerator random_;
				std::mt19937 *gen_;

				std::normal_distribution<double> normal_;

				// This is the probabliity of true for this field
				std::map<QString, double> bool_map_;

				// This is for a string (either choice or general string)
				std::map<QString, StringChoices> string_choices_;

				// This is for a numeric value, the pair is mean and standard deviation
				std::map<QString, std::shared_ptr<std::normal_distribution<double>>> numeric_;

				std::map<QString, QVariant> db_params_;
			};
		}
	}
}
