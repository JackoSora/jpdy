#include "view/question_dialog.h"

QuestionDialog::QuestionDialog(GameController* controller, QWidget* parent)
    : QDialog(parent), game_controller(controller), current_row(0), current_col(0), answer_shown(false) {
    setup_ui();
    setModal(true);
    setStyleSheet("background-color: #1a1a2e; color: white;");
}

void QuestionDialog::setup_ui() {
    setFixedSize(600, 500);
    setWindowTitle("Jeopardy Question");
    
    main_layout = new QVBoxLayout(this);
    main_layout->setSpacing(20);
    main_layout->setContentsMargins(30, 30, 30, 30);
    
    points_label = new QLabel;
    points_label->setStyleSheet(
        "QLabel { color: #ffd700; font-size: 24px; font-weight: bold; text-align: center; }"
    );
    points_label->setAlignment(Qt::AlignCenter);
    
    category_label = new QLabel;
    category_label->setStyleSheet(
        "QLabel { color: #87ceeb; font-size: 18px; font-weight: bold; text-align: center; }"
    );
    category_label->setAlignment(Qt::AlignCenter);
    
    
    question_display = new QTextEdit;
    question_display->setReadOnly(true);
    question_display->setMinimumHeight(120);
    question_display->setStyleSheet(
        "QTextEdit { background-color: #16213e; border: 3px solid #0f3460; "
        "padding: 15px; font-size: 16px; border-radius: 10px; }"
    );
    
    
    show_answer_button = new QPushButton("Show Answer");
    show_answer_button->setStyleSheet(
        "QPushButton { background-color: #16213e; color: #ffd700; border: 2px solid #0f3460; "
        "padding: 10px 20px; border-radius: 5px; font-weight: bold; font-size: 14px; }"
        "QPushButton:hover { background-color: #0f3460; }"
    );
    
    answer_display = new QTextEdit;
    answer_display->setReadOnly(true);
    answer_display->setMinimumHeight(100);
    answer_display->setVisible(false);
    answer_display->setStyleSheet(
        "QTextEdit { background-color: #2d4a22; border: 3px solid #4caf50; "
        "padding: 15px; font-size: 16px; border-radius: 10px; color: #e8f5e8; }"
    );
    
    button_layout = new QHBoxLayout;
    
    correct_button = new QPushButton("Correct (+points)");
    correct_button->setVisible(false);
    correct_button->setStyleSheet(
        "QPushButton { background-color: #4caf50; color: white; border: none; "
        "padding: 10px 20px; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background-color: #45a049; }"
    );
    
    incorrect_button = new QPushButton("Incorrect (-points)");
    incorrect_button->setVisible(false);
    incorrect_button->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; border: none; "
        "padding: 10px 20px; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background-color: #da190b; }"
    );
    
    close_button = new QPushButton("Close");
    close_button->setStyleSheet(
        "QPushButton { background-color: #16213e; color: white; border: 2px solid #0f3460; "
        "padding: 10px 20px; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background-color: #0f3460; }"
    );
    
    button_layout->addWidget(correct_button);
    button_layout->addWidget(incorrect_button);
    button_layout->addStretch();
    button_layout->addWidget(close_button);
    
    
    main_layout->addWidget(points_label);
    main_layout->addWidget(category_label);
    main_layout->addWidget(question_display);
    main_layout->addWidget(show_answer_button);
    main_layout->addWidget(answer_display);
    main_layout->addLayout(button_layout);
    
    
    connect(show_answer_button, &QPushButton::clicked, this, &QuestionDialog::show_answer);
    connect(correct_button, &QPushButton::clicked, this, &QuestionDialog::mark_correct);
    connect(incorrect_button, &QPushButton::clicked, this, &QuestionDialog::mark_incorrect);
    connect(close_button, &QPushButton::clicked, this, &QuestionDialog::close_dialog);
}

void QuestionDialog::show_question(size_t row, size_t col) {
    current_row = row;
    current_col = col;
    answer_shown = false;
    
    const board* board = game_controller->get_board();
    if (!board) return;
    
    const cell& game_cell = board->get_cell(row, col);
    
    points_label->setText(QString("$%1").arg(game_cell.get_points()));
    category_label->setText(QString("Category: %1").arg(QString::fromStdString(board->get_category(col))));
    
    question_display->setText(QString::fromStdString(game_cell.get_question()));
    
    show_answer_button->setVisible(true);
    answer_display->setVisible(false);
    correct_button->setVisible(false);
    incorrect_button->setVisible(false);
}

void QuestionDialog::show_answer() {
    if (answer_shown) return;
    
    const board* board = game_controller->get_board();
    if (!board) return;
    
    const cell& game_cell = board->get_cell(current_row, current_col);
    
    answer_display->setText(QString::fromStdString(game_cell.get_answer()));
    answer_display->setVisible(true);
    
    show_answer_button->setVisible(false);
    correct_button->setVisible(true);
    incorrect_button->setVisible(true);
    
    answer_shown = true;
}

void QuestionDialog::mark_correct() {
    const board* board = game_controller->get_board();
    if (!board) return;
    
    int points = board->get_cell(current_row, current_col).get_points();
    game_controller->add_to_score(points);
    close_dialog();
}

void QuestionDialog::mark_incorrect() {
    const board* board = game_controller->get_board();
    if (!board) return;
    
    int points = board->get_cell(current_row, current_col).get_points();
    game_controller->subtract_from_score(points);
    close_dialog();
}

void QuestionDialog::close_dialog() {
    accept();
}
