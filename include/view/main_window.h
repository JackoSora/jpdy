#pragma once
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include "controller/game_controller.h"

class ConfigWidget;
class GameBoardWidget;
class QuestionDialog;
class TeamWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    GameController* game_controller;
    QStackedWidget* stacked_widget;
    ConfigWidget* config_widget;
    GameBoardWidget* game_board_widget;
    QuestionDialog* question_dialog;
    TeamWidget* team_widget;
    
    QLabel* score_label;
    QPushButton* mode_toggle_button;
    
    void setup_ui();
    void setup_menu();
    void create_widgets();
    void setup_connections();

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() = default;

private slots:
    void on_mode_changed(GameMode new_mode);
    void on_score_changed(int new_score);
    void toggle_mode();
    void show_question_dialog(size_t row, size_t col);
};

#endif