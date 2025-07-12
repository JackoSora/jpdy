#include "view/main_window.h"
#include "view/config_widget.h"
#include "view/game_board_widget.h"
#include "view/question_dialog.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), game_controller(nullptr), stacked_widget(nullptr),
      config_widget(nullptr), game_board_widget(nullptr), question_dialog(nullptr),
      score_label(nullptr), mode_toggle_button(nullptr) {
    
    setStyleSheet("background-color: #1a1a2e; color: white;");
    resize(1200, 800);
    setWindowTitle("Jacko's Jeopardy");
    
    game_controller = new GameController(this);
    setup_ui();
    setup_menu();
    create_widgets();
    setup_connections();
}

void MainWindow::setup_ui() {
    QWidget* central_widget = new QWidget;
    setCentralWidget(central_widget);
    
    QVBoxLayout* main_layout = new QVBoxLayout(central_widget);
    
    
    QHBoxLayout* status_layout = new QHBoxLayout;
    
    score_label = new QLabel("Score: 0");
    score_label->setStyleSheet("font-size: 18px; font-weight: bold; color: #ffd700;");
    
    mode_toggle_button = new QPushButton("Start Game");
    mode_toggle_button->setStyleSheet(
        "QPushButton { background-color: #16213e; color: white; border: 2px solid #0f3460; "
        "padding: 8px 16px; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background-color: #0f3460; }"
        "QPushButton:pressed { background-color: #e94560; }"
    );
    
    status_layout->addWidget(score_label);
    status_layout->addStretch();
    status_layout->addWidget(mode_toggle_button);
    
    
    stacked_widget = new QStackedWidget;
    
    main_layout->addLayout(status_layout);
    main_layout->addWidget(stacked_widget);
}

void MainWindow::setup_menu() {
    QMenuBar* menu_bar = menuBar();
    menu_bar->setStyleSheet("QMenuBar { background-color: #16213e; color: white; }");
    
    QMenu* game_menu = menu_bar->addMenu("Game");
    
    QAction* config_action = new QAction("Configure Board", this);
    QAction* start_game_action = new QAction("Start Game", this);
    QAction* reset_action = new QAction("Reset Game", this);
    
    game_menu->addAction(config_action);
    game_menu->addAction(start_game_action);
    game_menu->addSeparator();
    game_menu->addAction(reset_action);
    
    connect(config_action, &QAction::triggered, [this]() {
        game_controller->start_config_mode();
    });
    
    connect(start_game_action, &QAction::triggered, [this]() {
        game_controller->start_game_mode();
    });
    
    connect(reset_action, &QAction::triggered, [this]() {
        game_controller->reset_game();
    });
}

void MainWindow::create_widgets() {
    config_widget = new ConfigWidget(game_controller, this);
    game_board_widget = new GameBoardWidget(game_controller, this);
    question_dialog = new QuestionDialog(game_controller, this);
    
    stacked_widget->addWidget(config_widget);
    stacked_widget->addWidget(game_board_widget);
    
    
    stacked_widget->setCurrentWidget(config_widget);
}

void MainWindow::setup_connections() {
    connect(game_controller, &GameController::mode_changed, this, &MainWindow::on_mode_changed);
    connect(game_controller, &GameController::score_changed, this, &MainWindow::on_score_changed);
    connect(mode_toggle_button, &QPushButton::clicked, this, &MainWindow::toggle_mode);
    connect(game_board_widget, &GameBoardWidget::cell_selected, this, &MainWindow::show_question_dialog);
}

void MainWindow::on_mode_changed(GameMode new_mode) {
    switch (new_mode) {
        case GameMode::CONFIG:
            stacked_widget->setCurrentWidget(config_widget);
            mode_toggle_button->setText("Start Game");
            score_label->setVisible(false);
            break;
        case GameMode::PLAYING:
            stacked_widget->setCurrentWidget(game_board_widget);
            mode_toggle_button->setText("Configure Board");
            score_label->setVisible(true);
            break;
    }
}

void MainWindow::on_score_changed(int new_score) {
    score_label->setText(QString("Score: %1").arg(new_score));
}

void MainWindow::toggle_mode() {
    if (game_controller->get_current_mode() == GameMode::CONFIG) {
        game_controller->start_game_mode();
    } else {
        game_controller->start_config_mode();
    }
}

void MainWindow::show_question_dialog(size_t row, size_t col) {
    question_dialog->show_question(row, col);
    question_dialog->exec();
}
