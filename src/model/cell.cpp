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
}

