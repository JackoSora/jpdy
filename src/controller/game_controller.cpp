#include "controller/game_controller.h"

GameController::GameController(QObject* parent)
    : QObject(parent) {
    game_state = std::make_unique<GameState>();
}

void GameController::start_config_mode() {
    game_state->start_config_mode();
    emit mode_changed(game_state->get_current_mode());
}

void GameController::start_game_mode() {
    game_state->start_game_mode();
    emit mode_changed(game_state->get_current_mode());
    emit score_changed(game_state->get_current_team_score());
    emit team_changed(game_state->get_current_team());
}

GameMode GameController::get_current_mode() const {
    return game_state->get_current_mode();
}

void GameController::configure_board_size(size_t rows, size_t cols) {
    game_state->configure_board_size(rows, cols);
    emit board_changed();
}

void GameController::set_category_name(size_t col, const std::string& name) {
    game_state->set_category_name(col, name);
    emit board_changed();
}

void GameController::set_question_answer(size_t row, size_t col, const std::string& question, const std::string& answer) {
    game_state->set_question_answer(row, col, question, answer);
    emit board_changed();
}

bool GameController::select_cell(size_t row, size_t col) {
    if (game_state->select_cell(row, col)) {
        emit cell_selected(row, col);
        return true;
    }
    return false;
}

void GameController::add_to_score(int points) {
    game_state->add_to_current_team_score(points);
    emit score_changed(game_state->get_current_team_score());
}

void GameController::subtract_from_score(int points) {
    game_state->subtract_from_current_team_score(points);
    emit score_changed(game_state->get_current_team_score());
}

int GameController::get_current_team_score() const {
    return game_state->get_current_team_score();
}

void GameController::reset_game() {
    game_state->reset_game();
    emit score_changed(game_state->get_current_team_score());
    emit board_changed();
    emit team_changed(game_state->get_current_team());
}

const board* GameController::get_board() const {
    return game_state->get_board();
}

const std::vector<team>& GameController::get_teams() const {
    return game_state->get_teams();
}

const team& GameController::get_current_team() const {
    return game_state->get_current_team();
}

bool GameController::add_team(const std::string& team_name) {
    return game_state->add_team(team_name);
}

void GameController::switch_to_next_team() {
    game_state->switch_to_next_team();
    emit team_changed(game_state->get_current_team());
}

void GameController::set_team_name(size_t team_index, const std::string& new_name) {
    game_state->set_team_name(team_index, new_name);
    emit team_changed(game_state->get_current_team());
}

// Point stealing functionality
bool GameController::can_current_team_attempt(size_t row, size_t col) const {
    return game_state->can_current_team_attempt(row, col);
}

void GameController::mark_current_team_attempted(size_t row, size_t col) {
    game_state->mark_current_team_attempted(row, col);
}

bool GameController::switch_to_next_available_team(size_t row, size_t col) {
    bool has_next_team = game_state->switch_to_next_available_team(row, col);
    if (has_next_team) {
        emit team_changed(game_state->get_current_team());
    }
    return has_next_team;
}

size_t GameController::get_current_team_index() const {
    return game_state->get_current_team_index();
}

bool GameController::is_question_in_progress(size_t row, size_t col) const {
    return game_state->is_question_in_progress(row, col);
}

void GameController::complete_question(size_t row, size_t col) {
    game_state->complete_question(row, col);
}
