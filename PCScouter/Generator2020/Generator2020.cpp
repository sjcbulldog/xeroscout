#include "Generator2020.h"
#include "BooleanFormItem.h"
#include "ImageFormItem.h"
#include "NumericFormItem.h"
#include "UpDownFormItem.h"
#include "TextFormItem.h"
#include "ChoiceFormItem.h"

#include <QDebug>
#include <random>
#include <set>
#include <array>

using namespace xero::scouting::datamodel;

static QRandomGenerator random;

static const char* names[] =
{
"Ruairidh Mcdonald",
"Arron Novak",
"Macauley Reyes",
"Owain Spencer",
"Vincent Osborn",
"Gary Harrison",
"Jana Kouma",
"Shayan Fleming",
"Rebeca Dickinson",
"Ellesha Bourne",
"Rosa Gough",
"Kashif Moore",
"Sufyaan Hunter",
"Tiah Hodson",
"Lucien Rahman",
"Barry Hurst",
"Shazia Wicks",
"Victoria Mejia",
"Ffion Yang",
"Niam Whitley",
"Loren Diaz",
"Saarah Coffey",
"Mathilda Dickerson",
"Rubi Schultz",
"Levison Floyd"
};
static int name_count = sizeof(names) / sizeof(names[0]);


ScoutingDataMapPtr generateTeam(std::shared_ptr<const DataModelTeam> team, std::shared_ptr<const ScoutingForm> form)
{
    int value;
    ScoutingDataMapPtr data = std::make_shared<ScoutingDataMap>();

    int db = random.bounded(100);
    int type = random.bounded(100);
    int neo = 0;
    int cim = 0;
    int falcon = 0;
    int other = 0;

    std::normal_distribution<double> dist(140.0, 20.0);
    data->insert_or_assign("weight", static_cast<int>(dist(random)));
    
    value = random.bounded(name_count);
    data->insert_or_assign("team_scouter", names[value]);

    value = random.bounded(1000);
    if (value < 500)
        data->insert_or_assign("language", "Java");
    else if (value < 950)
        data->insert_or_assign("language", "C++");
    else if (value < 990)
        data->insert_or_assign("language", "LV");
    else
        data->insert_or_assign("language", "Python");

    data->insert_or_assign("cpanel_under", random.bounded(1000) < 500);
    data->insert_or_assign("cpanel_rotate", random.bounded(1000) < 100);
    data->insert_or_assign("cpanel_position", random.bounded(1000) < 50);

    if (db < 60) {
        // West coast
        int total = 4;
        if (random.bounded(100) > 70)
            total = 6;

        data->insert_or_assign("db_type", "West Coast");
        data->insert_or_assign("db_wheels", 6);
        if (type < 20) {
            falcon = total;
        }
        else if (type < 60) {
            neo = total;
        }
        else {
            cim = total;
        }
    }
    else if (db < 80) {
        // Swerve
        data->insert_or_assign("db_type", "Swerve");
        data->insert_or_assign("db_wheels", 4);
        neo = 8;
    }
    else if (db < 97) {
        // Mecanum
        data->insert_or_assign("db_type", "Mecanum");
        data->insert_or_assign("db_wheels", 4);
        neo = 4;
    }
    else {
        // Other
        data->insert_or_assign("db_type", "Other");
        data->insert_or_assign("db_wheels", 4);
        cim = 4;
    }

    data->insert_or_assign("db_cims", cim);
    data->insert_or_assign("db_neos", neo);
    data->insert_or_assign("db_falcons", falcon);
    data->insert_or_assign("db_others", other);

    data->insert_or_assign("collect_ground", random.bounded(1000) < 950);
    data->insert_or_assign("score_lowgoal", random.bounded(1000) < 500);
    data->insert_or_assign("score_highgoal", random.bounded(1000) < 800);
    data->insert_or_assign("score_trench", random.bounded(1000) < 300);

    value = random.bounded(1000);
    if (value < 100) {
        // High level auto
        data->insert_or_assign("auto_line", true);
        if (random.bounded(1000) < 500)
            data->insert_or_assign("auto_perferred_start_pos", "Far Left");
        else
            data->insert_or_assign("auto_perferred_start_pos", "Right");

        data->insert_or_assign("auto_max_high", random.bounded(6, 10));
        data->insert_or_assign("auto_max_low", 0);
    }
    else if (value < 800)
    {
        // Nominal auto
        data->insert_or_assign("auto_line", true);
        data->insert_or_assign("auto_perferred_start_pos", "Center");
        data->insert_or_assign("auto_max_low", 3);
        data->insert_or_assign("auto_max_high", 0);
    }
    else
    {
        if (random.bounded(1000) < 500)
            data->insert_or_assign("auto_perferred_start_pos", "Center");
        else
            data->insert_or_assign("auto_perferred_start_pos", "Left");

        // Rookie auto
        if (random.bounded(1000) < 800)
            data->insert_or_assign("auto_line", true);
        else
            data->insert_or_assign("auto_line", false);
        data->insert_or_assign("auto_max_low", 0);
        data->insert_or_assign("auto_max_high", 0);
    }

    value = random.bounded(1000);

    if (value < 300) {
        // Strong climber
        data->insert_or_assign("climb_center", true);
        data->insert_or_assign("climb_sides", true);
        data->insert_or_assign("climb_tilt", true);
    }
    else if (value < 800) {
        // Ok climber
        data->insert_or_assign("climb_center", false);
        data->insert_or_assign("climb_sides", true);
        data->insert_or_assign("climb_tilt", false);
    }
    else {
        data->insert_or_assign("climb_center", false);
        data->insert_or_assign("climb_sides", false);
        data->insert_or_assign("climb_tilt", false);
    }

    if (random.bounded(1000) < 50)
        data->insert_or_assign("climb_leveler", true);
    else
        data->insert_or_assign("climb_leveler", false);

    value = random.bounded(1000);
    if (value < 10)
        data->insert_or_assign("climb_assists", 2);
    else if (value < 50)
        data->insert_or_assign("climb_assists", 1);
    else
        data->insert_or_assign("climb_assists", 0);

    for (auto f : form->fields()) {
        auto it = data->find(f->name());
        assert(it != data->end());
    }

    return data;
}

static void generateAutoHigh(ScoutingDataMapPtr data, const QString &pos)
{
    int value = 0;
    int high = 0;
    int low = 0;
    int eject = 0;

    for (int i = 1; i <= 12; i++)
    {
        QString bname = "auto__b" + QString::number(i);
        data->insert_or_assign(bname, false);
    }

    data->insert_or_assign("auto__starting_position", pos);

    if (pos == "Right")
    {
        high += 6;
        data->insert_or_assign("auto__b8", true);
        data->insert_or_assign("auto__b9", true);
        data->insert_or_assign("auto__b10", true);

        if (random.bounded(1000) < 50)
        {
            high += 2;
            data->insert_or_assign("auto__b6", true);
            data->insert_or_assign("auto__b7", true);
        }
    }
    else if (pos == "Far Left")
    {
        high += 5;
        data->insert_or_assign("auto__b1", true);
        data->insert_or_assign("auto__b2", true);

        value = random.bounded(1000);
        if (value < 20)
        {
            high += 5;
            data->insert_or_assign("auto__b3", true);
            data->insert_or_assign("auto__b4", true);
            data->insert_or_assign("auto__b5", true);
            data->insert_or_assign("auto__b6", true);
            data->insert_or_assign("auto__b7", true);
        }
        else if (value < 250)
        {
            high += 3;
            data->insert_or_assign("auto__b3", true);
            data->insert_or_assign("auto__b4", true);
            data->insert_or_assign("auto__b5", true);
        }
        else if (value < 500)
        {
            high += 2;
            data->insert_or_assign("auto__b3", true);
            data->insert_or_assign("auto__b4", true);
        }
        else if (value < 750)
        {
            high += 1;
            data->insert_or_assign("auto__b3", true);
        }
    }

    data->insert_or_assign("auto__line", true);
    data->insert_or_assign("auto__highgoal", high);
    data->insert_or_assign("auto__lowgoal", low);
    data->insert_or_assign("auto__eject", eject);
}

static void generateAutoMedium(ScoutingDataMapPtr data, const QString& pos)
{
    int value = 0;
    int high = 0;
    int low = 0;
    int eject = 0;

    for (int i = 1; i <= 12; i++)
    {
        QString bname = "auto__b" + QString::number(i);
        data->insert_or_assign(bname, false);
    }

    data->insert_or_assign("auto__starting_position", pos);
    high += 3;

    if (random.bounded(1000) < 100)
    {
        eject++;
        high--;
    }

    data->insert_or_assign("auto__line", true);
    data->insert_or_assign("auto__highgoal", high);
    data->insert_or_assign("auto__lowgoal", low);
    data->insert_or_assign("auto__eject", eject);
}

static void generateAutoLow(ScoutingDataMapPtr data, const QString& pos)
{
    int value;
    int high = 0;
    int low = 0;
    int eject = 0;

    for (int i = 1; i <= 12; i++)
    {
        QString bname = "auto__b" + QString::number(i);
        data->insert_or_assign(bname, false);
    }

    data->insert_or_assign("auto__starting_position", pos);

    low = random.bounded(4);

    if (random.bounded(1000) < 50)
        data->insert_or_assign("auto__line", false);
    else
        data->insert_or_assign("auto__line", true);
    data->insert_or_assign("auto__highgoal", high);
    data->insert_or_assign("auto__lowgoal", low);
    data->insert_or_assign("auto__eject", eject);
}

static void generateAuto(std::vector<ScoutingDataMapPtr> data, std::shared_ptr<const DataModelMatch> match, Alliance c)
{
    int value = random.bounded(1000);

    if (value < 50) {
        generateAutoMedium(data[0], "Center");
        generateAutoHigh(data[1], "Far Left");
        generateAutoHigh(data[2], "Right");
    }
    else if (value < 250) {
        generateAutoLow(data[0], "Center");
        generateAutoMedium(data[1], "Far Left");
        generateAutoHigh(data[2], "Right");
    }
    else if (value < 500)
    {
        generateAutoMedium(data[0], "Center");
        generateAutoMedium(data[1], "Far Left");
        generateAutoHigh(data[2], "Right");
    }
    else if (value < 750)
    {
        generateAutoLow(data[0], "Center");
        generateAutoMedium(data[1], "Far Left");
        generateAutoMedium(data[2], "Right");
    }
    else
    {
        generateAutoLow(data[0], "Center");
        generateAutoLow(data[1], "Far Left");
        generateAutoMedium(data[2], "Right");
    }
}

static void generateTeleop(std::vector<ScoutingDataMapPtr> data, std::shared_ptr<const DataModelMatch> match, Alliance c)
{
    int totalballs = 0;
    int value;

    for (int i = 0; i < 3; i++)
    {
        std::normal_distribution<double> dist(15.0, 10.0);
        value = static_cast<int>(dist(random));
        if (value < 0)
            value = 0;
        data[i]->insert_or_assign("teleop__highgoal", value);
        totalballs += value;

        dist = std::normal_distribution<double>(5.0, 5.0);
        value = static_cast<int>(dist(random));
        if (value < 0)
            value = 0;
        data[i]->insert_or_assign("teleop__lowgoal", value);
        totalballs += value;

        dist = std::normal_distribution<double>(3.0, 3.0);
        value = static_cast<int>(dist(random));
        if (value < 0)
            value = 0;
        data[i]->insert_or_assign("teleop__ejected", value);

        data[i]->insert_or_assign("teleop__spin", false);
        data[i]->insert_or_assign("teleop__position", false);
    }

    bool spin = false;
    if (totalballs > 29)
    {
        //
        // We can spin, so give us a 30% chance
        //
        if (random.bounded(1000) < 300)
        {
            int which = random.bounded(3);
            data[which]->insert_or_assign("teleop__spin", true);
            spin = true;
        }
    }

    if (totalballs > 49 && spin)
    {
        if (random.bounded(1000) < 200)
        {
            int which = random.bounded(3);
            data[which]->insert_or_assign("teleop__position", true);
        }
    }
}

static void generateEndgame(std::vector<ScoutingDataMapPtr> data, std::shared_ptr<const DataModelMatch> match, Alliance c)
{
    int value;
    value = random.bounded(1000);

    for (int i = 0; i < 3; i++)
    {
        data[i]->insert_or_assign("endgame__number_assisted", static_cast<int>(0));
        data[i]->insert_or_assign("endgame__level", false);
        data[i]->insert_or_assign("endgame__was_assisted", false);
        data[i]->insert_or_assign("endgame__position", "nothing");
    }

    if (value < 10)
    {
        // Two assist climb
        data[0]->insert_or_assign("endgame__number_assisted", 2);
        data[1]->insert_or_assign("endgame__was_assisted", true);
        data[2]->insert_or_assign("endgame__was_assisted", true);
        data[0]->insert_or_assign("endgame__position", "endgame__pzero");
        data[1]->insert_or_assign("endgame__position", "endgame__pzero");
        data[2]->insert_or_assign("endgame__position", "endgame__pzero");
        data[0]->insert_or_assign("endgame__level", true);
        data[1]->insert_or_assign("endgame__level", true);
        data[2]->insert_or_assign("endgame__level", true);
    }
    else if (value < 30)
    {
        // One assist climb
        data[0]->insert_or_assign("endgame__number_assisted", 1);
        data[1]->insert_or_assign("endgame__was_assisted", true);
        data[0]->insert_or_assign("endgame__position", "endgame__pzero");
        data[1]->insert_or_assign("endgame__position", "endgame__pzero");
        data[0]->insert_or_assign("endgame__level", true);
        data[1]->insert_or_assign("endgame__level", true);
        data[2]->insert_or_assign("endgame__level", true);
    }
    else if (value < 100)
    {
        // Triple climb with level
        data[0]->insert_or_assign("endgame__position", "endgame__pneg3");
        data[1]->insert_or_assign("endgame__position", "endgame__pzero");
        data[2]->insert_or_assign("endgame__position", "endgame__ppos3");
        data[0]->insert_or_assign("endgame__level", true);
        data[1]->insert_or_assign("endgame__level", true);
        data[2]->insert_or_assign("endgame__level", true);
    }
    else if (value < 200)
    {
        // Triple climb without level
        data[0]->insert_or_assign("endgame__position","endgame__pneg3");
        data[1]->insert_or_assign("endgame__position","endgame__pzero");
        data[2]->insert_or_assign("endgame__position","endgame__ppos3");
    }
    else if (value < 300)
    {
        // Double climb with level
        data[0]->insert_or_assign("endgame__position","endgame__pneg2");
        data[1]->insert_or_assign("endgame__position","endgame__ppos2");
        data[0]->insert_or_assign("endgame__level", true);
        data[1]->insert_or_assign("endgame__level", true);
        data[2]->insert_or_assign("endgame__level", true);
    }
    else if (value < 400)
    {
        // Double climb
        data[0]->insert_or_assign("endgame__position", "endgame__pneg3");
        data[1]->insert_or_assign("endgame__position", "endgame__ppos1");
    }
    else if (value < 500)
    {
        // Single climb with level
        data[0]->insert_or_assign("endgame__position", "endgame__ppos1");
        data[0]->insert_or_assign("endgame__level", true);
        data[1]->insert_or_assign("endgame__level", true);
        data[2]->insert_or_assign("endgame__level", true);
    }
    else if (value < 800)
    {
        // Single climb
        data[0]->insert_or_assign("endgame__position", "endgame__ppos1");
    }

    for (int i = 0; i < data.size(); i++)
    {
        auto it = data[i]->find("endgame__position");
        if (it->second.toString() == "nothing" && random.bounded(1000) < 800)
            data[i]->insert_or_assign("endgame__position", "parked");
    }
}

static std::vector<ScoutingDataMapPtr> generateHalfMatch(std::shared_ptr<const DataModelMatch> match, std::shared_ptr<const ScoutingForm> form, Alliance c)
{
    int value;
    std::vector<ScoutingDataMapPtr> data;

    data.push_back(std::make_shared<ScoutingDataMap>());
    data.push_back(std::make_shared<ScoutingDataMap>());
    data.push_back(std::make_shared<ScoutingDataMap>());


    //
    // Prematch
    //
    std::set<int> indexes;
    for (int i = 0; i < 3; i++)
    {
        do {
            value = random.bounded(name_count);
        } while (indexes.count(value) > 0);
        data[i]->insert_or_assign("match_scouter", names[value]);
    }

    //
    // Autonomous
    //
    generateAuto(data, match, c);


    //
    // Teleop
    //
    generateTeleop(data, match, c);

    //
    // End game
    //
    generateEndgame(data, match, c);

    return data;
}

std::vector<ScoutingDataMapPtr> generateMatch(std::shared_ptr<const DataModelMatch> match, std::shared_ptr<const ScoutingForm> form)
{
    std::vector<ScoutingDataMapPtr> ret;

    auto red = generateHalfMatch(match, form, Alliance::Red);
    ret.insert(ret.end(), red.begin(), red.end());

    auto blue = generateHalfMatch(match, form, Alliance::Blue);
    ret.insert(ret.end(), blue.begin(), blue.end());

    for (int i = 0; i < ret.size(); i++)
    {
        ret[i]->insert_or_assign("brownout", false);
        ret[i]->insert_or_assign("breakdown", false);

        if (random.bounded(1000) < 20)
            ret[i]->insert_or_assign("brownout", true);
        else if (random.bounded(1000) < 10)
            ret[i]->insert_or_assign("breakdown", true);

        int gathered = 0;
        for (int b = 1; b <= 12; b++)
        {
            auto it = ret[i]->find("auto__b" + QString::number(b));
            assert(it != ret[i]->end());

            if (it->second.toBool())
                gathered++;
        }

        ret[i]->insert_or_assign("auto__gathered", gathered);
    }

    assert(ret.size() == 6);
    for (int i = 0; i < ret.size(); i++)
    {
        auto data = ret[i];
        for (auto f : form->fields()) {
            auto it = data->find(f->name());
            assert(it != data->end());
        }
    }

    return ret;
}
