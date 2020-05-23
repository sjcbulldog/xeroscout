#include "GameRandomProfile.h"
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			GameRandomProfile::GameRandomProfile()
			{
				gen_ = new std::mt19937();
			}

			GameRandomProfile::~GameRandomProfile()
			{
			}

			void GameRandomProfile::load(const QString& filename)
			{
				numeric_.clear();
				bool_map_.clear();
				string_choices_.clear();

				QFile file(filename);
				if (!file.open(QIODevice::ReadOnly))
				{
					QString msg = "could not open file'" + filename + "' for reading";
					emit error(msg);
					return;
				}

				QByteArray data = file.readAll();
				QJsonParseError err;
				QJsonDocument doc;

				doc = QJsonDocument::fromJson(data, &err);
				if (doc.isNull())
				{
					QString msg = "file'" + filename + "' was not a valid JSON file";
					emit error(msg);
					return;
				}

				QJsonArray array = doc.array();

				for (int i = 0; i < array.size(); i++)
				{
					if (!array[i].isObject())
					{
						QString msg = "entry " + QString::number(i + 1) + ": is not an object";
						emit error(msg);
						continue;
					}

					QJsonObject obj = array[i].toObject();
					if (!obj.contains("type"))
					{
						QString msg = "entry " + QString::number(i + 1) + ": does not have a 'type' field";
						emit error(msg);
						continue;
					}

					if (!obj.value("type").isString())
					{
						QString msg = "entry " + QString::number(i + 1) + ": has a 'type' field but it is not a string";
						emit error(msg);
						continue;
					}

					if (!obj.contains("tag"))
					{
						QString msg = "entry " + QString::number(i + 1) + ": does not have a 'tag' field";
						emit error(msg);
						continue;
					}

					if (!obj.value("tag").isString())
					{
						QString msg = "entry " + QString::number(i + 1) + ": has a 'tag' field but it is not a string";
						emit error(msg);
						continue;
					}

					QString type = obj.value("type").toString();
					QString tag = obj.value("tag").toString();

					if (type == "choice")
					{
						if (!obj.contains("choices") || !obj.value("choices").isArray() || !isArrayString(obj.value("choices").toArray()))
						{
							QString msg = "entry " + QString::number(i + 1) + ": the 'choices' field is not a string array";
							emit error(msg);
							continue;
						}

						if (!obj.contains("profile") || !obj.value("profile").isArray() || !isArrayDouble(obj.value("profile").toArray()))
						{
							QString msg = "entry " + QString::number(i + 1) + ": the 'profile' field is not a double array";
							emit error(msg);
							continue;
						}

						const QJsonArray& charray = obj.value("choices").toArray();
						const QJsonArray& prarray = obj.value("profile").toArray();
						if (charray.size() != prarray.size())
						{
							QString msg = "entry " + QString::number(i + 1) + ": the 'profile' array and the 'choices' array are not the same size";
							emit error(msg);
							continue;
						}

						StringChoices sc;
						for (int i = 0; i < charray.size(); i++)
						{
							sc.push_back(std::make_pair(charray.at(i).toString(), prarray.at(i).toDouble()));
						}

						string_choices_.insert_or_assign(tag, sc);
					}
					else if (type == "boolean")
					{
						if (!obj.contains("true"))
						{
							QString msg = "entry " + QString::number(i + 1) + ": does not have a 'true' field";
							emit error(msg);
							continue;
						}

						if (!obj.value("true").isDouble())
						{
							QString msg = "entry " + QString::number(i + 1) + ": has a 'true' field but it is not a double";
							emit error(msg);
							continue;
						}

						bool_map_.insert_or_assign(tag, obj.value("true").toDouble());
					}
					else if (type == "numeric")
					{
						if (!obj.contains("mean"))
						{
							QString msg = "entry " + QString::number(i + 1) + ": does not have a 'mean' field";
							emit error(msg);
							continue;
						}

						if (!obj.value("mean").isDouble())
						{
							QString msg = "entry " + QString::number(i + 1) + ": has a 'mean' field but it is not a double";
							emit error(msg);
							continue;
						}

						if (!obj.contains("stddev"))
						{
							QString msg = "entry " + QString::number(i + 1) + ": does not have a 'stddev' field";
							emit error(msg);
							continue;
						}

						if (!obj.value("stddev").isDouble())
						{
							QString msg = "entry " + QString::number(i + 1) + ": has a 'stddev' field but it is not a double";
							emit error(msg);
							continue;
						}

						double mean = obj.value("mean").toDouble();
						double stddev = obj.value("stddev").toDouble();

						auto dist = std::make_shared<std::normal_distribution<double>>(mean, stddev);
						numeric_.insert_or_assign(tag, dist);
					}
				}
			}

			bool GameRandomProfile::isArrayDouble(const QJsonArray& array)
			{
				for (auto a : array)
				{
					if (!a.isDouble())
						return false;
				}

				return true;
			}

			bool GameRandomProfile::isArrayString(const QJsonArray& array)
			{
				for (auto a : array)
				{
					if (!a.isString())
						return false;
				}

				return true;
			}

			QVariant GameRandomProfile::generateRandomBool(const QString& field)
			{
				QVariant ret;

				auto itp = db_params_.find(field);
				if (itp != db_params_.end())
					return itp->second;

				auto it = bool_map_.find(field);
				if (it == bool_map_.end())
				{
					qDebug() << "no profile for field '" << field << "'";
					ret = QVariant(random_.bounded(0, 2) ? true : false);
				}
				else
				{
					ret = QVariant(random_.generateDouble() < it->second);
				}

				return ret;
			}

			QVariant GameRandomProfile::generateRandomChoice(const QString& field, const QStringList &choices)
			{
				QVariant ret;

				if (field.startsWith("db_") && db_params_.size() == 0)
					generateDriveBase(choices);

				auto itp = db_params_.find(field);
				if (itp != db_params_.end())
					return itp->second;

				auto it = string_choices_.find(field);
				if (it == string_choices_.end())
				{
					qDebug() << "no profile for field '" << field << "'";
					int which = random_.bounded(0, choices.size());
					ret = QVariant(choices.at(which));
				}
				else
				{
					const StringChoices& sc = it->second;
					double total = 0.0;
					for (auto c : sc)
						total += c.second;

					if (std::fabs(total - 1.0) > 0.1)
					{
						qDebug() << "bad set of choices for field '" << field << "', probabilities don't add to one";
						qDebug() << "using random choice";

						int which = random_.bounded(0, choices.size());
						ret = QVariant(choices.at(which));
					}
					else
					{
						bool found = false;
						double v = random_.generateDouble();
						for (auto entry : sc)
						{
							if (v <= entry.second)
							{
								found = true;
								ret = QVariant(entry.first);
								break;
							}

							v -= entry.second;
						}

						if (!found)
						{
							ret = QVariant(sc.back().second);
						}
					}
				}

				return ret;
			}

			QVariant GameRandomProfile::generateRandomNumeric(const QString& field, int minv, int maxv)
			{
				QVariant ret;

				auto itp = db_params_.find(field);
				if (itp != db_params_.end())
					return itp->second;

				auto it = numeric_.find(field);
				if (it == numeric_.end())
				{
					qDebug() << "no profile for field '" << field << "'";
					ret = QVariant(random_.bounded(minv, maxv + 1));
				}
				else
				{
					double v = (*it->second)(*gen_);
					int vi = static_cast<int>(v);
					if (vi < minv)
						vi = minv;
					else if (vi > maxv)
						vi = maxv;

					ret = QVariant(vi);
				}

				return ret;
			}

			QVariant GameRandomProfile::generateRandomText(const QString& field)
			{
				QVariant ret;

				auto it = string_choices_.find(field);
				if (it == string_choices_.end())
				{
					qDebug() << "no profile for field '" << field << "'";
					ret = QVariant("RandomText");
				}
				else
				{
					const StringChoices& sc = it->second;
					double total = 0.0;
					for (auto c : sc)
						total += c.second;

					if (std::fabs(total - 1.0) > 0.1)
					{
						qDebug() << "bad set of choices for field '" << field << "', probabilities don't add to one";
						qDebug() << "using random choice";

						ret = QVariant("RandomText");
					}
					else
					{
						bool found = false;
						double v = random_.generateDouble();
						for (auto entry : sc)
						{
							if (v <= entry.second)
							{
								found = true;
								ret = QVariant(entry.first);
								break;
							}

							v -= entry.second;
						}

						if (!found)
						{
							ret = QVariant(sc.back().second);
						}
					}
				}

				return ret;
			}

			// db_type, db_wheels
			// db_cims, db_neos, db_falcons, db_others
			void GameRandomProfile::generateDriveBase(const QStringList &list)
			{
				int r = random_.bounded(0, 100);

				if (r < 70)
				{
					// West coast
					for (const QString& str : list)
					{
						if (str.toLower().contains("west"))
						{
							db_params_.insert_or_assign("db_type", QVariant(str));
							break;
						}
					}
					db_params_.insert_or_assign("db_wheels", QVariant(6));

					r = random_.bounded(0, 100);
					if (r < 25)
					{
						db_params_.insert_or_assign("db_cims", QVariant(4));
						db_params_.insert_or_assign("db_neos", QVariant(0));
						db_params_.insert_or_assign("db_falcons", QVariant(0));
						db_params_.insert_or_assign("db_others", QVariant(0));

					}
					else if (r < 50)
					{
						db_params_.insert_or_assign("db_cims", QVariant(6));
						db_params_.insert_or_assign("db_neos", QVariant(0));
						db_params_.insert_or_assign("db_falcons", QVariant(0));
						db_params_.insert_or_assign("db_others", QVariant(0));
					}
					else if (r < 75)
					{
						db_params_.insert_or_assign("db_cims", QVariant(0));
						db_params_.insert_or_assign("db_neos", QVariant(4));
						db_params_.insert_or_assign("db_falcons", QVariant(0));
						db_params_.insert_or_assign("db_others", QVariant(0));
					}
					else
					{
						db_params_.insert_or_assign("db_cims", QVariant(0));
						db_params_.insert_or_assign("db_neos", QVariant(0));
						db_params_.insert_or_assign("db_falcons", QVariant(4));
						db_params_.insert_or_assign("db_others", QVariant(0));
					}
				}
				else if (r < 95)
				{
					// Swerve
					for (const QString& str : list)
					{
						if (str.toLower().contains("swerve"))
						{
							db_params_.insert_or_assign("db_type", QVariant(str));
							break;
						}
					}
					db_params_.insert_or_assign("db_wheels", QVariant(4));

					r = random_.bounded(0, 100);
					if (r < 10)
					{
						db_params_.insert_or_assign("db_cims", QVariant(8));
						db_params_.insert_or_assign("db_neos", QVariant(0));
						db_params_.insert_or_assign("db_falcons", QVariant(0));
						db_params_.insert_or_assign("db_others", QVariant(0));

					}
					else if (r < 55)
					{
						db_params_.insert_or_assign("db_cims", QVariant(0));
						db_params_.insert_or_assign("db_neos", QVariant(8));
						db_params_.insert_or_assign("db_falcons", QVariant(0));
						db_params_.insert_or_assign("db_others", QVariant(0));
					}
					else
					{
						db_params_.insert_or_assign("db_cims", QVariant(0));
						db_params_.insert_or_assign("db_neos", QVariant(4));
						db_params_.insert_or_assign("db_falcons", QVariant(4));
						db_params_.insert_or_assign("db_others", QVariant(0));
					}
				}
				else
				{
					// Mecanum
					for (const QString& str : list)
					{
						if (str.toLower().contains("mecanum"))
						{
							db_params_.insert_or_assign("db_type", QVariant(str));
							break;
						}
					}
					db_params_.insert_or_assign("db_wheels", QVariant(4));

					r = random_.bounded(0, 100);
					if (r < 10)
					{
						db_params_.insert_or_assign("db_cims", QVariant(4));
						db_params_.insert_or_assign("db_neos", QVariant(0));
						db_params_.insert_or_assign("db_falcons", QVariant(0));
						db_params_.insert_or_assign("db_others", QVariant(0));

					}
					else if (r < 55)
					{
						db_params_.insert_or_assign("db_cims", QVariant(0));
						db_params_.insert_or_assign("db_neos", QVariant(4));
						db_params_.insert_or_assign("db_falcons", QVariant(0));
						db_params_.insert_or_assign("db_others", QVariant(0));
					}
					else
					{
						db_params_.insert_or_assign("db_cims", QVariant(0));
						db_params_.insert_or_assign("db_neos", QVariant(0));
						db_params_.insert_or_assign("db_falcons", QVariant(4));
						db_params_.insert_or_assign("db_others", QVariant(0));
					}
				}
			}
		}
	}
}
