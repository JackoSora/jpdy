#pragma once
#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "controller/game_controller.h"

class GameBoardWidget : public QWidget {
    Q_OBJECT

private:
    GameController* game_controller;
    QVBoxLayout* main_layout;
    QGridLayout* board_layout;
    
    std::vector<QLabel*> category_labels;
    std::vector<std::vector<QPushButton*>> cell_buttons;
    
    void setup_ui();
    void rebuild_board();
    void clear_board();
    void update_cell_button(size_t row, size_t col);

public:
    explicit GameBoardWidget(GameController* controller, QWidget* parent = nullptr);
    ~GameBoardWidget() = default;

private slots:
    void on_cell_clicked();
    void on_board_changed();

signals:
    void cell_selected(size_t row, size_t col);
};
