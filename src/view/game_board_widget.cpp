#include "view/game_board_widget.h"
#include <QPushButton>

GameBoardWidget::GameBoardWidget(GameController* controller, QWidget* parent)
    : QWidget(parent), game_controller(controller) {
    setup_ui();
    
    connect(game_controller, &GameController::board_changed, this, &GameBoardWidget::on_board_changed);
    rebuild_board();
}

void GameBoardWidget::setup_ui() {
    main_layout = new QVBoxLayout(this);
    main_layout->setSpacing(10);
    main_layout->setContentsMargins(20, 20, 20, 20);
    
    setStyleSheet("background-color: #1a1a2e;");
}

void GameBoardWidget::rebuild_board() {
    clear_board();
    
    const board* board = game_controller->get_board();
    if (!board) return;
    
    size_t rows = board->get_rows();
    size_t cols = board->get_cols();
    
    
    QWidget* board_widget = new QWidget;
    board_layout = new QGridLayout(board_widget);
    board_layout->setSpacing(5);
    
    category_labels.clear();
    cell_buttons.clear();
    
    
    for (size_t col = 0; col < cols; ++col) {
        QLabel* category_label = new QLabel(QString::fromStdString(board->get_category(col)));
        category_label->setStyleSheet(
            "QLabel { background-color: #16213e; color: #ffd700; border: 3px solid #0f3460; "
            "padding: 15px; font-weight: bold; font-size: 16px; text-align: center; }"
        );
        category_label->setAlignment(Qt::AlignCenter);
        category_label->setMinimumHeight(60);
        
        category_labels.push_back(category_label);
        board_layout->addWidget(category_label, 0, static_cast<int>(col));
    }
    
    cell_buttons.resize(rows);
    for (size_t row = 0; row < rows; ++row) {
        cell_buttons[row].resize(cols);
        for (size_t col = 0; col < cols; ++col) {
            QPushButton* cell_button = new QPushButton;
            cell_button->setMinimumSize(150, 100);
            cell_button->setProperty("row", static_cast<int>(row));
            cell_button->setProperty("col", static_cast<int>(col));
            
            cell_buttons[row][col] = cell_button;
            board_layout->addWidget(cell_button, static_cast<int>(row + 1), static_cast<int>(col));
            
            connect(cell_button, &QPushButton::clicked, this, &GameBoardWidget::on_cell_clicked);
            update_cell_button(row, col);
        }
    }
    
    main_layout->addWidget(board_widget);
}

void GameBoardWidget::clear_board() {
    if (board_layout) {
        QLayoutItem* item;
        while ((item = board_layout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete board_layout->parentWidget();
        board_layout = nullptr;
    }
    category_labels.clear();
    cell_buttons.clear();
}

void GameBoardWidget::update_cell_button(size_t row, size_t col) {
    const board* board = game_controller->get_board();
    if (!board || row >= cell_buttons.size() || col >= cell_buttons[row].size()) return;
    
    const cell& game_cell = board->get_cell(row, col);
    QPushButton* button = cell_buttons[row][col];
    
    if (game_cell.get_is_revealed()) {
        button->setText("USED");
        button->setEnabled(false);
        button->setStyleSheet(
            "QPushButton { background-color: #333; color: #666; border: 3px solid #555; "
            "border-radius: 10px; font-weight: bold; font-size: 14px; }"
        );
    } else {
        button->setText(QString("$%1").arg(game_cell.get_points()));
        button->setEnabled(true);
        button->setStyleSheet(
            "QPushButton { background-color: #16213e; color: #ffd700; border: 3px solid #0f3460; "
            "border-radius: 10px; font-weight: bold; font-size: 18px; }"
            "QPushButton:hover { background-color: #0f3460; }"
            "QPushButton:pressed { background-color: #e94560; }"
        );
    }
}

void GameBoardWidget::on_cell_clicked() {
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;
    
    int row = button->property("row").toInt();
    int col = button->property("col").toInt();
    
    if (game_controller->select_cell(static_cast<size_t>(row), static_cast<size_t>(col))) {
        update_cell_button(static_cast<size_t>(row), static_cast<size_t>(col));
        emit cell_selected(static_cast<size_t>(row), static_cast<size_t>(col));
    }
}

void GameBoardWidget::on_board_changed() {
    if (game_controller->get_current_mode() == GameMode::PLAYING) {
        
        const board* board = game_controller->get_board();
        if (board) {
            for (size_t row = 0; row < board->get_rows(); ++row) {
                for (size_t col = 0; col < board->get_cols(); ++col) {
                    if (row < cell_buttons.size() && col < cell_buttons[row].size()) {
                        update_cell_button(row, col);
                    }
                }
            }
        }
    } else {
        rebuild_board();
    }
}
