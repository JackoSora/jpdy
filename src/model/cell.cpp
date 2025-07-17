#include "../../include/model/cell.h"

void cell::set_question(const std::string& question) {
    this->question = question;
}

void cell::set_answer(const std::string& answer) {
    this->answer = answer;
}

void cell::set_points(int points) {
    this->points = points;
}

void cell::set_is_revealed(bool is_revealed) {
    this->is_revealed = is_revealed;
}

const std::string& cell::get_answer() const {
    return this->answer;
}

const std::string& cell::get_question() const {
    return this->question;
}

int cell::get_points() const {
    return this->points;
}

bool cell::get_is_revealed() const {
    return this->is_revealed;
}

void cell::reveal() {
    this->is_revealed = true;
}

void cell::reset() {
    this->is_revealed = false;
    this->attempted_teams.clear();
}

// Team attempt tracking methods
void cell::add_attempted_team(size_t team_index) {
    attempted_teams.insert(team_index);
}

bool cell::has_team_attempted(size_t team_index) const {
    return attempted_teams.find(team_index) != attempted_teams.end();
}

const std::set<size_t>& cell::get_attempted_teams() const {
    return attempted_teams;
}

void cell::clear_attempted_teams() {
    attempted_teams.clear();
}

