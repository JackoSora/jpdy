#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include "controller/game_controller.h"

class QuestionDialog : public QDialog {
    Q_OBJECT

private:
    GameController* game_controller;
    size_t current_row, current_col;
    
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
    
    bool answer_shown;
    
    void setup_ui();

public:
    explicit QuestionDialog(GameController* controller, QWidget* parent = nullptr);
    ~QuestionDialog() = default;
    
    void show_question(size_t row, size_t col);

private slots:
    void show_answer();
    void mark_correct();
    void mark_incorrect();
    void close_dialog();
};
