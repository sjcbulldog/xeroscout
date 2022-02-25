#include "MatchScheduleGenerator.h"

void MatchScheduleGenerator::generate()
{
	double score;

	for (auto t : model_->teams()) {
		teams_.push_back(t->number());
	}

	createInitialSchedule();
	score = computeScore();

	while (score > 10.0) {
		pertubate();
		score = computeScore();
	}

	for (int i = 0; i < matches_.size(); i++) {
		QString key = "qm-" + QString::number(i + 1);
		auto m = model_->addMatch(key, "qm", 0, i + 1, 0);

		for (int j = 0; j < 6; j++) {
			int team = matches_[i][j];
			auto t = model_->findTeamByNumber(team);

			xero::scouting::datamodel::Alliance a = xero::scouting::datamodel::Alliance::Red;
			if (j > 2)
				a = xero::scouting::datamodel::Alliance::Blue;

			model_->addTeamToMatch(key, a, (j % 3) + 1, t->key(), team);
		}
	}
}

void MatchScheduleGenerator::createInitialSchedule()
{
	int index = 0;
	int match = 0;
	int slot = 0;

	while (match < matches_.size()) {
		slot = 0;
		while (slot < 6) {
			matches_[match][slot] = teams_[index];
			slot++;
			index = (index + 1) % teams_.size();
		}
		match++;
	}
}

double MatchScheduleGenerator::computeScore()
{
	return 0.0;
}

void MatchScheduleGenerator::pertubate()
{

}