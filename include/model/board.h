#pragma once
#include "model/cell.h"
#include <vector>
#include <string>

class board {
private:
    std::vector<std::vector<cell>> cells;
    std::vector<std::string> categories;
    size_t rows;
    size_t cols;

public:
    explicit board(size_t rows = 5, size_t cols = 6);
    
    
    void resize_board(size_t new_rows, size_t new_cols);
    void set_category(size_t col, const std::string& category);
    void set_cell_content(size_t row, size_t col, const std::string& question, const std::string& answer);
    
    
    [[nodiscard]] size_t get_rows() const;
    [[nodiscard]] size_t get_cols() const;
    [[nodiscard]] const std::string& get_category(size_t col) const;
    [[nodiscard]] const cell& get_cell(size_t row, size_t col) const;
    [[nodiscard]] cell& get_cell(size_t row, size_t col);
    
    
    void reveal_cell(size_t row, size_t col);
    void reset_board();
    [[nodiscard]] bool is_valid_position(size_t row, size_t col) const;
    
    
    [[nodiscard]] int get_cell_points(size_t row) const;
    
    ~board() = default;
};
