#include "view/config_widget.h"
#include <QHeaderView>
#include <QMessageBox>

ConfigWidget::ConfigWidget(GameController* controller, QWidget* parent)
    : QWidget(parent), game_controller(controller) {
    setup_ui();
    
    connect(game_controller, &GameController::board_changed, this, &ConfigWidget::on_board_changed);
    rebuild_config_grid();
}

void ConfigWidget::setup_ui() {
    main_layout = new QVBoxLayout(this);
    
    create_size_controls();
    
    
    config_scroll_area = new QScrollArea;
    config_content_widget = new QWidget;
    config_grid_layout = new QGridLayout(config_content_widget);
    
    config_scroll_area->setWidget(config_content_widget);
    config_scroll_area->setWidgetResizable(true);
    config_scroll_area->setStyleSheet(
        "QScrollArea { border: 2px solid #16213e; background-color: #0f3460; }"
    );
    
    main_layout->addWidget(config_scroll_area);
}

void ConfigWidget::create_size_controls() {
    QWidget* size_widget = new QWidget;
    size_layout = new QHBoxLayout(size_widget);
    
    QLabel* rows_label = new QLabel("Rows:");
    rows_spinbox = new QSpinBox;
    rows_spinbox->setMinimum(3);
    rows_spinbox->setMaximum(10);
    rows_spinbox->setValue(5);
    
    QLabel* cols_label = new QLabel("Columns:");
    cols_spinbox = new QSpinBox;
    cols_spinbox->setMinimum(3);
    cols_spinbox->setMaximum(8);
    cols_spinbox->setValue(6);
    
    apply_size_button = new QPushButton("Apply Size");
    apply_size_button->setStyleSheet(
        "QPushButton { background-color: #16213e; color: white; border: 2px solid #0f3460; "
        "padding: 8px 16px; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background-color: #0f3460; }"
    );
    
    size_layout->addWidget(rows_label);
    size_layout->addWidget(rows_spinbox);
    size_layout->addWidget(cols_label);
    cols_spinbox->setStyleSheet("QSpinBox { background-color: #16213e; color: #ffd700; border: 2px solid #0f3460; "
                                 "padding: 8px; font-weight: bold; font-size: 14px; }");
    size_layout->addWidget(cols_spinbox);
    size_layout->addWidget(apply_size_button);
    size_layout->addStretch();
    
    main_layout->addWidget(size_widget);
    
    connect(apply_size_button, &QPushButton::clicked, this, &ConfigWidget::apply_board_size);
}

void ConfigWidget::rebuild_config_grid() {
    clear_config_grid();
    
    const board* board = game_controller->get_board();
    if (!board) return;
    
    size_t rows = board->get_rows();
    size_t cols = board->get_cols();
    
    category_inputs.clear();
    cell_edit_buttons.clear();
    
    for (size_t col = 0; col < cols; ++col) {
        QLineEdit* category_input = new QLineEdit(QString::fromStdString(board->get_category(col)));
        category_input->setStyleSheet(
            "QLineEdit { background-color: #16213e; color: #ffd700; border: 2px solid #0f3460; "
            "padding: 8px; font-weight: bold; font-size: 14px; }"
        );
        category_inputs.push_back(category_input);
        config_grid_layout->addWidget(category_input, 0, static_cast<int>(col));
        
        connect(category_input, QOverload<const QString&>::of(&QLineEdit::textChanged),
                [this, col](const QString&) { update_category(static_cast<int>(col)); });
    }
    
    cell_edit_buttons.resize(rows);
    for (size_t row = 0; row < rows; ++row) {
        cell_edit_buttons[row].resize(cols);
        for (size_t col = 0; col < cols; ++col) {
            QPushButton* edit_button = new QPushButton(QString("Edit\n$%1").arg(board->get_cell_points(row)));
            edit_button->setMinimumSize(120, 80);
            
            cell_edit_buttons[row][col] = edit_button;
            config_grid_layout->addWidget(edit_button, static_cast<int>(row + 1), static_cast<int>(col));
            
            connect(edit_button, &QPushButton::clicked, 
                    [this, row, col]() { edit_cell(static_cast<int>(row), static_cast<int>(col)); });
            
            // Update the button style based on cell content
            update_cell_button_style(row, col);
        }
    }
}

void ConfigWidget::update_cell_button_style(size_t row, size_t col) {
    const board* board = game_controller->get_board();
    if (!board || row >= cell_edit_buttons.size() || col >= cell_edit_buttons[row].size()) {
        return;
    }
    
    const cell& current_cell = board->get_cell(row, col);
    QPushButton* button = cell_edit_buttons[row][col];
    
    QString question = QString::fromStdString(current_cell.get_question()).trimmed();
    QString answer = QString::fromStdString(current_cell.get_answer()).trimmed();
    
    bool has_question = !question.isEmpty();
    bool has_answer = !answer.isEmpty();
    
    QString base_style = 
        "QPushButton { color: white; border-radius: 5px; font-weight: bold; }";
    
    QString hover_style = "QPushButton:hover { background-color: #0f3460; }";
    
    if (has_question && has_answer) {
        // Green border and background for complete cells (question + answer)
        button->setStyleSheet(base_style + 
            "QPushButton { background-color: #1a4d1a; "
            "border: 3px solid #00ff00; }" + hover_style);
    } else if (has_question) {
        // Purple border and background for cells with only question
        button->setStyleSheet(base_style + 
            "QPushButton { background-color: #4d1a4d; "
            "border: 3px solid #8a2be2; }" + hover_style);
    } else {
        // Default style for empty cells
        button->setStyleSheet(base_style + 
            "QPushButton { background-color: #16213e; "
            "border: 2px solid #0f3460; }" + hover_style);
    }
}

void ConfigWidget::clear_config_grid() {
    QLayoutItem* item;
    while ((item = config_grid_layout->takeAt(0)) != nullptr) {
        if (QWidget* widget = item->widget()) {
            delete widget;
        }
        delete item;
    }
    category_inputs.clear();
    cell_edit_buttons.clear();
}

void ConfigWidget::apply_board_size() {
    size_t new_rows = static_cast<size_t>(rows_spinbox->value());
    size_t new_cols = static_cast<size_t>(cols_spinbox->value());
    game_controller->configure_board_size(new_rows, new_cols);
}

void ConfigWidget::edit_cell(int row, int col) {
    CellEditDialog* dialog = new CellEditDialog(game_controller, static_cast<size_t>(row), static_cast<size_t>(col), this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    
    // Connect to update the cell appearance when dialog is closed/saved
    connect(dialog, &CellEditDialog::cell_updated, 
            [this, row, col]() { update_cell_button_style(static_cast<size_t>(row), static_cast<size_t>(col)); });
    
    dialog->show();
}

void ConfigWidget::update_category(int col) {
    if (col >= 0 && col < static_cast<int>(category_inputs.size())) {
        std::string category_text = category_inputs[static_cast<size_t>(col)]->text().toStdString();
        game_controller->set_category_name(static_cast<size_t>(col), category_text);
    }
}

void ConfigWidget::on_board_changed() {
    rebuild_config_grid();
}

CellEditDialog::CellEditDialog(GameController* controller, size_t row, size_t col, QWidget* parent)
    : QWidget(parent), row(row), col(col), game_controller(controller) {
    
    setWindowTitle(QString("Edit Cell (%1, %2)").arg(row + 1).arg(col + 1));
    setFixedSize(400, 300);
    setStyleSheet("background-color: #1a1a2e; color: white;");
    
    layout = new QVBoxLayout(this);
    
    const board* board = game_controller->get_board();
    const cell& current_cell = board->get_cell(row, col);
    
    title_label = new QLabel(QString("Editing $%1 Question").arg(board->get_cell_points(row)));
    title_label->setStyleSheet("font-size: 16px; font-weight: bold; color: #ffd700;");
    title_label->setAlignment(Qt::AlignCenter);
    
    QLabel* question_label = new QLabel("Question:");
    question_edit = new QTextEdit(QString::fromStdString(current_cell.get_question()));
    question_edit->setMaximumHeight(80);
    question_edit->setStyleSheet("background-color: #16213e; border: 2px solid #0f3460; padding: 5px;");
    
    QLabel* answer_label = new QLabel("Answer:");
    answer_edit = new QTextEdit(QString::fromStdString(current_cell.get_answer()));
    answer_edit->setMaximumHeight(80);
    answer_edit->setStyleSheet("background-color: #16213e; border: 2px solid #0f3460; padding: 5px;");
    
    QHBoxLayout* button_layout = new QHBoxLayout;
    save_button = new QPushButton("Save");
    cancel_button = new QPushButton("Cancel");
    
    QString button_style = 
        "QPushButton { background-color: #16213e; color: white; border: 2px solid #0f3460; "
        "padding: 8px 16px; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background-color: #0f3460; }";
    
    save_button->setStyleSheet(button_style);
    cancel_button->setStyleSheet(button_style);
    
    button_layout->addWidget(save_button);
    button_layout->addWidget(cancel_button);
    
    layout->addWidget(title_label);
    layout->addWidget(question_label);
    layout->addWidget(question_edit);
    layout->addWidget(answer_label);
    layout->addWidget(answer_edit);
    layout->addLayout(button_layout);
    
    connect(save_button, &QPushButton::clicked, this, &CellEditDialog::save_cell);
    connect(cancel_button, &QPushButton::clicked, this, &CellEditDialog::cancel_edit);
}

void CellEditDialog::save_cell() {
    std::string question = question_edit->toPlainText().toStdString();
    std::string answer = answer_edit->toPlainText().toStdString();
    game_controller->set_question_answer(row, col, question, answer);
    emit cell_updated();  
    close();
}

void CellEditDialog::cancel_edit() {
    close();
}
