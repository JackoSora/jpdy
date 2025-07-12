#pragma once
#include "model/board.h"
#include <QObject>
#include <memory>

enum class GameMode {
    CONFIG,
    PLAYING
};

class GameController : public QObject {
    Q_OBJECT

private:
    std::unique_ptr<board> game_board;
    GameMode current_mode;
    int current_score;

public:
    explicit GameController(QObject* parent = nullptr);
    ~GameController() = default;

    
    void start_config_mode();
    void start_game_mode();
    [[nodiscard]] GameMode get_current_mode() const;
    
    
    void configure_board_size(size_t rows, size_t cols);
    void set_category_name(size_t col, const std::string& name);
    void set_question_answer(size_t row, size_t col, const std::string& question, const std::string& answer);
    
    
    bool select_cell(size_t row, size_t col);
    void add_to_score(int points);
    void subtract_from_score(int points);
    [[nodiscard]] int get_score() const;
    void reset_game();
    
    
    [[nodiscard]] const board* get_board() const;
    
signals:
    void board_changed();
    void mode_changed(GameMode new_mode);
    void score_changed(int new_score);
    void cell_selected(size_t row, size_t col);
};
