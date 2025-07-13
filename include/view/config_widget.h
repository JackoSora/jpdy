#pragma once
#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QTextEdit>
#include <QScrollArea>
#include "controller/game_controller.h"

class ConfigWidget : public QWidget {
    Q_OBJECT

private:
    GameController* game_controller;
    QVBoxLayout* main_layout;
    QHBoxLayout* size_layout;
    QSpinBox* rows_spinbox;
    QSpinBox* cols_spinbox;
    QPushButton* apply_size_button;
    
    QScrollArea* config_scroll_area;
    QWidget* config_content_widget;
    QGridLayout* config_grid_layout;
    
    std::vector<QLineEdit*> category_inputs;
    std::vector<std::vector<QPushButton*>> cell_edit_buttons;
    
    void setup_ui();
    void create_size_controls();
    void rebuild_config_grid();
    void clear_config_grid();
    void update_cell_button_style(size_t row, size_t col);

public:
    explicit ConfigWidget(GameController* controller, QWidget* parent = nullptr);
    ~ConfigWidget() = default;

private slots:
    void apply_board_size();
    void edit_cell(int row, int col);
    void update_category(int col);
    void on_board_changed();
};

class CellEditDialog : public QWidget {
    Q_OBJECT

private:
    size_t row, col;
    GameController* game_controller;
    QVBoxLayout* layout;
    QLabel* title_label;
    QTextEdit* question_edit;
    QTextEdit* answer_edit;
    QPushButton* save_button;
    QPushButton* cancel_button;

public:
    explicit CellEditDialog(GameController* controller, size_t row, size_t col, QWidget* parent = nullptr);
    ~CellEditDialog() = default;

signals:
    void cell_updated();

private slots:
    void save_cell();
    void cancel_edit();
};
