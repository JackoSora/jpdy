#pragma once

#include "model/game_state.h"
#include <QObject>
#include <memory>

class GameController : public QObject {
    Q_OBJECT

private:
    std::unique_ptr<GameState> game_state;

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
    [[nodiscard]] int get_current_team_score() const;
    void reset_game();

    
    // Team management
    [[nodiscard]] const board* get_board() const;
    [[nodiscard]] const std::vector<team>& get_teams() const;
    [[nodiscard]] const team& get_current_team() const;
    bool add_team(const std::string& team_name);
    void switch_to_next_team();
    void set_team_name(size_t team_index, const std::string& new_name);
    
    // Point stealing functionality  
    bool can_current_team_attempt(size_t row, size_t col) const;
    void mark_current_team_attempted(size_t row, size_t col);
    bool switch_to_next_available_team(size_t row, size_t col);
    [[nodiscard]] size_t get_current_team_index() const;

signals:
    void board_changed();
    void mode_changed(GameMode new_mode);
    void score_changed(int new_score);
    void cell_selected(size_t row, size_t col);
    void team_changed(const team& current_team);
};
