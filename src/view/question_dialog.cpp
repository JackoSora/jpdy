#include "view/question_dialog.h"
#include <stdexcept>

QuestionDialog::QuestionDialog(GameController* controller, QWidget* parent)
    : QDialog(parent), game_controller(controller), current_row(0), current_col(0), 
      answer_shown(false), point_stealing_mode(false), animation_manager(nullptr), transition_manager(nullptr) {
    setup_ui();
    setup_managers();
    setModal(true);
    setStyleSheet("background-color: #1a1a2e; color: white;");
}

QuestionDialog::~QuestionDialog() {
    if (animation_manager) {
        animation_manager->cleanup_animations();
        delete animation_manager;
    }
    if (transition_manager) {
        delete transition_manager;
    }
}

void QuestionDialog::setup_managers() {
    // Create animation manager
    animation_manager = new AnimationManager(this, this);
    
    // Create transition manager
    transition_manager = new DialogTransitionManager(this, this);
    
    // Connect animation signals
    connect(animation_manager, &AnimationManager::correct_animation_finished,
            this, &QuestionDialog::on_correct_animation_finished);
    connect(animation_manager, &AnimationManager::incorrect_animation_finished,
            this, &QuestionDialog::on_incorrect_animation_finished);
    
    // Connect transition signals
    connect(transition_manager, &DialogTransitionManager::transition_finished,
            this, &QuestionDialog::on_transition_finished);
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

void QuestionDialog::enable_buttons(bool enabled) {
    correct_button->setEnabled(enabled);
    incorrect_button->setEnabled(enabled);
    close_button->setEnabled(enabled);
}

void QuestionDialog::reset_ui_state() {
    answer_shown = false;
    point_stealing_mode = false;
    
    // Reset UI components
    show_answer_button->setVisible(true);
    show_answer_button->setEnabled(true);
    answer_display->setVisible(false);
    correct_button->setVisible(false);
    incorrect_button->setVisible(false);
    
    // Ensure buttons are enabled
    enable_buttons(true);
}

void QuestionDialog::show_question(size_t row, size_t col) {
    current_row = row;
    current_col = col;
    
    // Clean up any running animations and reset state
    if (animation_manager) {
        animation_manager->cleanup_animations();
    }
    
    // Reset dialog opacity
    if (transition_manager) {
        transition_manager->reset_opacity();
    }
    
    // Reset UI state
    reset_ui_state();
    
    // Load question data
    const board* board = game_controller->get_board();
    if (!board) return;
    
    const cell& game_cell = board->get_cell(row, col);
    const team& current_team = game_controller->get_current_team();
    
    points_label->setText(QString("$%1").arg(game_cell.get_points()));
    category_label->setText(QString("Category: %1 - %2's Turn")
                            .arg(QString::fromStdString(board->get_category(col)))
                            .arg(QString::fromStdString(current_team.get_name())));
    question_display->setText(QString::fromStdString(game_cell.get_question()));
    
    // Check if answer is already revealed for this question
    if (game_cell.is_answer_revealed()) {
        // Answer was already revealed, show it immediately
        answer_display->setText(QString::fromStdString(game_cell.get_answer()));
        answer_display->setVisible(true);
        show_answer_button->setVisible(false);
        correct_button->setVisible(true);
        incorrect_button->setVisible(true);
        answer_shown = true;
    }
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
    
    // Mark answer as revealed in the cell
    game_controller->mark_answer_revealed(current_row, current_col);
}

void QuestionDialog::mark_correct() {
    if (!animation_manager || animation_manager->is_animation_in_progress()) return;
    
    // Disable buttons during animation
    enable_buttons(false);
    
    // Get points for animation
    const board* board = game_controller->get_board();
    int points = board ? board->get_cell(current_row, current_col).get_points() : 0;
    
    try {
        animation_manager->play_correct_animation(points);
    } catch (const std::exception& e) {
        qWarning("Animation failed, falling back to immediate action: %s", e.what());
        enable_buttons(true);
        execute_correct_action();
    }
}

void QuestionDialog::mark_incorrect() {
    if (!animation_manager || animation_manager->is_animation_in_progress()) return;
    
    // Disable buttons during animation
    enable_buttons(false);
    
    // Get points for animation
    const board* board = game_controller->get_board();
    int points = board ? board->get_cell(current_row, current_col).get_points() : 0;
    
    try {
        animation_manager->play_incorrect_animation(points);
    } catch (const std::exception& e) {
        qWarning("Animation failed, falling back to immediate action: %s", e.what());
        enable_buttons(true);
        execute_incorrect_action();
    }
}

void QuestionDialog::on_correct_animation_finished() {
    enable_buttons(true);
    execute_correct_action();
}

void QuestionDialog::on_incorrect_animation_finished() {
    enable_buttons(true);
    execute_incorrect_action();
}

void QuestionDialog::execute_correct_action() {
    const board* board = game_controller->get_board();
    if (!board) return;
    
    int points = board->get_cell(current_row, current_col).get_points();
    game_controller->add_to_score(points);
    game_controller->complete_question(current_row, current_col);  // Mark question as completed
    game_controller->switch_to_next_team();
    
    // Start fade-out transition
    if (transition_manager) {
        transition_manager->fade_out_and_close();
    } else {
        close_dialog();
    }
}

void QuestionDialog::setup_for_next_team() {
    // Update the display for the new team attempting the question
    const team& current_team = game_controller->get_current_team();
    category_label->setText(QString("Category: %1 - %2's Turn to Steal!")
                            .arg(QString::fromStdString(game_controller->get_board()->get_category(current_col)))
                            .arg(QString::fromStdString(current_team.get_name())));
    
    // Check if answer was already revealed for this question
    const board* board = game_controller->get_board();
    const cell& game_cell = board->get_cell(current_row, current_col);
    
    if (game_cell.is_answer_revealed()) {
        // Answer was already revealed, keep it visible
        answer_shown = true;
        answer_display->setVisible(true);
        show_answer_button->setVisible(false);
        correct_button->setVisible(true);
        incorrect_button->setVisible(true);
    } else {
        // Answer hasn't been revealed yet, reset state
        answer_shown = false;
        answer_display->setVisible(false);
        show_answer_button->setVisible(true);
        show_answer_button->setEnabled(true);
        correct_button->setVisible(false);
        incorrect_button->setVisible(false);
    }
    
    // Re-enable buttons
    enable_buttons(true);
}

void QuestionDialog::execute_incorrect_action() {
    const board* board = game_controller->get_board();
    if (!board) return;
    
    int points = board->get_cell(current_row, current_col).get_points();
    game_controller->subtract_from_score(points);
    game_controller->mark_current_team_attempted(current_row, current_col);
    
    // Check if there are other teams that can attempt this question
    if (game_controller->switch_to_next_available_team(current_row, current_col)) {
        // Point stealing: setup for next team
        point_stealing_mode = true;
        setup_for_next_team();
    } else {
        // No more teams can attempt, close the question
        game_controller->complete_question(current_row, current_col);  // Mark question as completed
        game_controller->switch_to_next_team();  // Move to next team for next question
        
        // Start fade-out transition
        if (transition_manager) {
            transition_manager->fade_out_and_close();
        } else {
            close_dialog();
        }
    }
}

void QuestionDialog::on_transition_finished() {
    close_dialog();
}

void QuestionDialog::close_dialog() {
    accept();
}
