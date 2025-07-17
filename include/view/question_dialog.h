#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include "controller/game_controller.h"
#include "view/animation_manager.h"
#include "view/dialog_transition_manager.h"

class QuestionDialog : public QDialog {
    Q_OBJECT

private:
    // Core game data
    GameController* game_controller;
    size_t current_row, current_col;
    bool answer_shown;
    
    // UI components
    QVBoxLayout* main_layout;
    QLabel* points_label;
    QLabel* category_label;
    QTextEdit* question_display;
    QPushButton* show_answer_button;
    QTextEdit* answer_display;
    QHBoxLayout* button_layout;
    QPushButton* correct_button;
    QPushButton* incorrect_button;
    QPushButton* close_button;
    
    // Managers for separated concerns
    AnimationManager* animation_manager;
    DialogTransitionManager* transition_manager;
    
    // UI setup and management
    void setup_ui();
    void setup_managers();
    void reset_ui_state();
    void enable_buttons(bool enabled);
    
    // Game logic execution
    void execute_correct_action();
    void execute_incorrect_action();

public:
    explicit QuestionDialog(GameController* controller, QWidget* parent = nullptr);
    ~QuestionDialog();
    
    void show_question(size_t row, size_t col);

private slots:
    void show_answer();
    void mark_correct();
    void mark_incorrect();
    void close_dialog();
    
    // Animation completion handlers
    void on_correct_animation_finished();
    void on_incorrect_animation_finished();
    
    // Transition completion handler
    void on_transition_finished();
};
