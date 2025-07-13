#pragma once

#include "model/board.h"
#include "model/team.h"
#include <vector>
#include <memory>
#include <string>

enum class GameMode {
    CONFIG,
    PLAYING
};

class GameState {
private:
    std::unique_ptr<board> game_board;
    std::vector<team> teams;
    GameMode current_mode;
    int current_team_index;
    const size_t max_teams = 5;

public:
    explicit GameState(size_t rows = 5, size_t cols = 6);
    ~GameState() = default;

    
    void start_config_mode();
    void start_game_mode();
    [[nodiscard]] GameMode get_current_mode() const;

    
    void configure_board_size(size_t rows, size_t cols);
    void set_category_name(size_t col, const std::string& name);
    void set_question_answer(size_t row, size_t col, const std::string& question, const std::string& answer);
    [[nodiscard]] const board* get_board() const;

    
    bool select_cell(size_t row, size_t col);

    
    // Team management
    bool add_team(const std::string& team_name);
    void switch_to_next_team();
    void add_to_current_team_score(int points);
    void subtract_from_current_team_score(int points);
    [[nodiscard]] int get_current_team_score() const;
    [[nodiscard]] const team& get_current_team() const;
    [[nodiscard]] const std::vector<team>& get_teams() const;
    void set_team_name(size_t team_index, const std::string& new_name);

    // Game reset
    void reset_game();
};
