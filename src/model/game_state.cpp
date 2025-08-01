#include "model/game_state.h"

GameState::GameState(size_t rows, size_t cols)
    : current_mode(GameMode::CONFIG), current_team_index(0) {
    game_board = std::make_unique<board>(rows, cols);
    teams.emplace_back("Team 1");
    teams.emplace_back("Team 2");
    teams.emplace_back("Team 3");
}

void GameState::start_config_mode() {
    current_mode = GameMode::CONFIG;
}

void GameState::start_game_mode() {
    current_mode = GameMode::PLAYING;
    current_team_index = 0;
    for (auto& team : teams) {
        team.set_score(0);
    }
    game_board->reset_board();
}

GameMode GameState::get_current_mode() const {
    return current_mode;
}

void GameState::configure_board_size(size_t rows, size_t cols) {
    if (current_mode == GameMode::CONFIG) {
        game_board->resize_board(rows, cols);
    }
}

void GameState::set_category_name(size_t col, const std::string& name) {
    if (current_mode == GameMode::CONFIG) {
        game_board->set_category(col, name);
    }
}

void GameState::set_question_answer(size_t row, size_t col, const std::string& question, const std::string& answer) {
    if (current_mode == GameMode::CONFIG) {
        game_board->set_cell_content(row, col, question, answer);
    }
}

const board* GameState::get_board() const {
    return game_board.get();
}

bool GameState::select_cell(size_t row, size_t col) {
    if (current_mode == GameMode::PLAYING && game_board->is_valid_position(row, col)) {
        const cell& game_cell = game_board->get_cell(row, col);
        // Allow selection if cell is not revealed and not currently in progress
        if (!game_cell.get_is_revealed() && !is_question_in_progress(row, col)) {
            // Don't mark as revealed yet - only mark when question is completely finished
            return true;
        }
    }
    return false;
}

bool GameState::add_team(const std::string& team_name) {
    if (teams.size() < max_teams) {
        teams.emplace_back(team_name);
        return true;
    }
    return false;
}

void GameState::switch_to_next_team() {
    current_team_index = (current_team_index + 1) % teams.size();
}

void GameState::add_to_current_team_score(int points) {
    if (!teams.empty()) {
        teams[current_team_index].set_score(teams[current_team_index].get_score() + points);
    }
}

void GameState::subtract_from_current_team_score(int points) {
    if (!teams.empty()) {
        teams[current_team_index].set_score(teams[current_team_index].get_score() - points);
    }
}

int GameState::get_current_team_score() const {
    if (!teams.empty()) {
        return teams[current_team_index].get_score();
    }
    return 0;
}

const team& GameState::get_current_team() const {
    return teams[current_team_index];
}

const std::vector<team>& GameState::get_teams() const {
    return teams;
}

void GameState::set_team_name(size_t team_index, const std::string& new_name) {
    if (team_index < teams.size() && !new_name.empty()) {
        teams[team_index].set_name(new_name);
    }
}

void GameState::reset_game() {
    start_game_mode();
}

// Point stealing functionality
bool GameState::can_current_team_attempt(size_t row, size_t col) const {
    if (current_mode != GameMode::PLAYING || !game_board->is_valid_position(row, col)) {
        return false;
    }
    
    const cell& game_cell = game_board->get_cell(row, col);
    return !game_cell.has_team_attempted(current_team_index);
}

void GameState::mark_current_team_attempted(size_t row, size_t col) {
    if (current_mode == GameMode::PLAYING && game_board->is_valid_position(row, col)) {
        get_cell_mutable(row, col).add_attempted_team(current_team_index);
    }
}

bool GameState::switch_to_next_available_team(size_t row, size_t col) {
    if (current_mode != GameMode::PLAYING || !game_board->is_valid_position(row, col)) {
        return false;
    }
    
    // If there's only one team, no point stealing is possible
    if (teams.size() <= 1) {
        return false;
    }
    
    const cell& game_cell = game_board->get_cell(row, col);
    size_t original_team = current_team_index;
    
    // Try each team to find one that hasn't attempted this question
    for (size_t i = 0; i < teams.size(); ++i) {
        switch_to_next_team();
        if (!game_cell.has_team_attempted(current_team_index)) {
            return true;  // Found a team that can attempt
        }
        if (current_team_index == original_team) {
            break;  // We've cycled through all teams
        }
    }
    
    return false;  // No team available to attempt
}

size_t GameState::get_current_team_index() const {
    return current_team_index;
}

// Helper method to get mutable cell reference
cell& GameState::get_cell_mutable(size_t row, size_t col) {
    return game_board->get_cell(row, col);
}

bool GameState::is_question_in_progress(size_t row, size_t col) const {
    if (!game_board->is_valid_position(row, col)) {
        return false;
    }
    
    const cell& game_cell = game_board->get_cell(row, col);
    // Question is in progress if some teams have attempted but it's not revealed
    return !game_cell.get_attempted_teams().empty() && !game_cell.get_is_revealed();
}

void GameState::complete_question(size_t row, size_t col) {
    if (current_mode == GameMode::PLAYING && game_board->is_valid_position(row, col)) {
        game_board->reveal_cell(row, col);
    }
}
