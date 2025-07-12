#include "controller/game_controller.h"

GameController::GameController(QObject* parent) 
    : QObject(parent), current_mode(GameMode::CONFIG), current_score(0) {
    game_board = std::make_unique<board>(5, 6); 
}

void GameController::start_config_mode() {
    current_mode = GameMode::CONFIG;
    emit mode_changed(current_mode);
}

void GameController::start_game_mode() {
    current_mode = GameMode::PLAYING;
    current_score = 0;
    game_board->reset_board();
    emit mode_changed(current_mode);
    emit score_changed(current_score);
}

GameMode GameController::get_current_mode() const {
    return current_mode;
}

void GameController::configure_board_size(size_t rows, size_t cols) {
    if (current_mode == GameMode::CONFIG) {
        game_board->resize_board(rows, cols);
        emit board_changed();
    }
}

void GameController::set_category_name(size_t col, const std::string& name) {
    if (current_mode == GameMode::CONFIG) {
        game_board->set_category(col, name);
        emit board_changed();
    }
}

void GameController::set_question_answer(size_t row, size_t col, const std::string& question, const std::string& answer) {
    if (current_mode == GameMode::CONFIG) {
        game_board->set_cell_content(row, col, question, answer);
        emit board_changed();
    }
}

bool GameController::select_cell(size_t row, size_t col) {
    if (current_mode == GameMode::PLAYING && 
        game_board->is_valid_position(row, col) && 
        !game_board->get_cell(row, col).get_is_revealed()) {
        
        game_board->reveal_cell(row, col);
        emit cell_selected(row, col);
        return true;
    }
    return false;
}

void GameController::add_to_score(int points) {
    current_score += points;
    emit score_changed(current_score);
}

void GameController::subtract_from_score(int points) {
    current_score -= points;
    emit score_changed(current_score);
}

int GameController::get_score() const {
    return current_score;
}

void GameController::reset_game() {
    current_score = 0;
    game_board->reset_board();
    emit score_changed(current_score);
    emit board_changed();
}

const board* GameController::get_board() const {
    return game_board.get();
}

#include "game_controller.moc"
