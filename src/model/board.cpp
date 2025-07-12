#include "model/board.h"

board::board(size_t rows, size_t cols) : rows(rows), cols(cols) {
    cells.resize(rows, std::vector<cell>(cols));
    categories.resize(cols);
    
    
    for (size_t i = 0; i < cols; ++i) {
        categories[i] = "Category " + std::to_string(i + 1);
    }
    
    
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            cells[row][col].set_points(get_cell_points(row));
            cells[row][col].set_question("Default question for " + std::to_string((row + 1) * 100) + " points");
            cells[row][col].set_answer("Default answer for " + std::to_string((row + 1) * 100) + " points");
        }
    }
}

void board::resize_board(size_t new_rows, size_t new_cols) {
    rows = new_rows;
    cols = new_cols;
    
    cells.resize(rows, std::vector<cell>(cols));
    categories.resize(cols);
    
    
    for (size_t i = 0; i < cols; ++i) {
        if (categories[i].empty()) {
            categories[i] = "Category " + std::to_string(i + 1);
        }
    }
    
    
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            cells[row][col].set_points(get_cell_points(row));
            if (cells[row][col].get_question().empty()) {
                cells[row][col].set_question("Question for " + std::to_string((row + 1) * 100) + " points");
                cells[row][col].set_answer("Answer for " + std::to_string((row + 1) * 100) + " points");
            }
        }
    }
}

void board::set_category(size_t col, const std::string& category) {
    if (col < cols) {
        categories[col] = category;
    }
}

void board::set_cell_content(size_t row, size_t col, const std::string& question, const std::string& answer) {
    if (is_valid_position(row, col)) {
        cells[row][col].set_question(question);
        cells[row][col].set_answer(answer);
    }
}

size_t board::get_rows() const {
    return rows;
}

size_t board::get_cols() const {
    return cols;
}

const std::string& board::get_category(size_t col) const {
    static const std::string empty_category = "";
    if (col < cols) {
        return categories[col];
    }
    return empty_category;
}

const cell& board::get_cell(size_t row, size_t col) const {
    return cells[row][col];
}

cell& board::get_cell(size_t row, size_t col) {
    return cells[row][col];
}

void board::reveal_cell(size_t row, size_t col) {
    if (is_valid_position(row, col)) {
        cells[row][col].reveal();
    }
}

void board::reset_board() {
    for (auto& row : cells) {
        for (auto& cell : row) {
            cell.reset();
        }
    }
}

bool board::is_valid_position(size_t row, size_t col) const {
    return row < rows && col < cols;
}

int board::get_cell_points(size_t row) const {
    return static_cast<int>((row + 1) * 100);
}

