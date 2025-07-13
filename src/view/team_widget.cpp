#include "view/team_widget.h"
#include "controller/game_controller.h"
#include <QString>

// TeamDisplayWidget Implementation
TeamDisplayWidget::TeamDisplayWidget(const team* team_ptr, QWidget* parent)
    : QFrame(parent), team_data(team_ptr), name_label(nullptr), score_label(nullptr),
      name_edit(nullptr), is_current_team(false), is_editing_name(false) {
    setup_ui();
    update_display();
}

void TeamDisplayWidget::setup_ui() {
    setFrameStyle(QFrame::Box | QFrame::Raised);
    setLineWidth(2);
    setMinimumHeight(80);
    setMinimumWidth(200);
    
    QVBoxLayout* main_layout = new QVBoxLayout(this);
    main_layout->setSpacing(5);
    main_layout->setContentsMargins(10, 10, 10, 10);
    
    // Name display/edit
    QHBoxLayout* name_layout = new QHBoxLayout;
    
    name_label = new QLabel;
    name_label->setStyleSheet(
        "QLabel {"
        "font-size: 16px;"
        "font-weight: bold;"
        "color: #ffd700;"
        "padding: 5px;"
        "border-radius: 3px;"
        "}"
        "QLabel:hover {"
        "background-color: rgba(255, 215, 0, 0.2);"
        "cursor: pointer;"
        "}"
    );
    name_label->setAlignment(Qt::AlignCenter);
    name_label->installEventFilter(this);
    
    name_edit = new QLineEdit;
    name_edit->setStyleSheet(
        "QLineEdit {"
        "font-size: 16px;"
        "font-weight: bold;"
        "color: #ffd700;"
        "background-color: #16213e;"
        "border: 2px solid #0f3460;"
        "padding: 5px;"
        "border-radius: 3px;"
        "}"
    );
    name_edit->setAlignment(Qt::AlignCenter);
    name_edit->hide();
    
    name_layout->addWidget(name_label);
    name_layout->addWidget(name_edit);
    
    // Score display
    score_label = new QLabel;
    score_label->setStyleSheet(
        "QLabel {"
        "font-size: 14px;"
        "color: white;"
        "padding: 3px;"
        "}"
    );
    score_label->setAlignment(Qt::AlignCenter);
    
    main_layout->addLayout(name_layout);
    main_layout->addWidget(score_label);
    
    connect(name_edit, &QLineEdit::editingFinished, this, &TeamDisplayWidget::on_name_edit_finished);
}

void TeamDisplayWidget::update_display() {
    if (!team_data) return;
    
    name_label->setText(QString::fromStdString(team_data->get_name()));
    score_label->setText(QString("Score: %1").arg(team_data->get_score()));
    
    // Update styling based on current team status
    QString border_color = is_current_team ? "#e94560" : "#0f3460";
    QString bg_color = is_current_team ? "rgba(233, 69, 96, 0.1)" : "#1a1a2e";
    
    setStyleSheet(QString(
        "TeamDisplayWidget {"
        "background-color: %1;"
        "border: 2px solid %2;"
        "border-radius: 8px;"
        "}"
    ).arg(bg_color, border_color));
}

void TeamDisplayWidget::set_team_data(const team* team_ptr) {
    team_data = team_ptr;
    update_display();
}

void TeamDisplayWidget::set_current_team(bool is_current) {
    is_current_team = is_current;
    update_display();
}

void TeamDisplayWidget::update_team_info() {
    update_display();
}

void TeamDisplayWidget::start_name_editing() {
    if (!team_data || is_editing_name) return;
    
    is_editing_name = true;
    name_edit->setText(QString::fromStdString(team_data->get_name()));
    name_label->hide();
    name_edit->show();
    name_edit->setFocus();
    name_edit->selectAll();
}

void TeamDisplayWidget::finish_name_editing() {
    if (!is_editing_name) return;
    
    is_editing_name = false;
    name_edit->hide();
    name_label->show();
    
    QString new_name = name_edit->text().trimmed();
    if (!new_name.isEmpty() && new_name != QString::fromStdString(team_data->get_name())) {
        emit name_change_requested(team_data, new_name);
    }
}

bool TeamDisplayWidget::eventFilter(QObject* obj, QEvent* event) {
    if (obj == name_label && event->type() == QEvent::MouseButtonPress) {
        start_name_editing();
        return true;
    }
    return QFrame::eventFilter(obj, event);
}

void TeamDisplayWidget::on_name_clicked() {
    start_name_editing();
}

void TeamDisplayWidget::on_name_edit_finished() {
    finish_name_editing();
}

void TeamDisplayWidget::on_name_edit_escape() {
    if (is_editing_name) {
        is_editing_name = false;
        name_edit->hide();
        name_label->show();
    }
}

void TeamDisplayWidget::keyPressEvent(QKeyEvent* event) {
    if (is_editing_name && event->key() == Qt::Key_Escape) {
        on_name_edit_escape();
        return;
    }
    QFrame::keyPressEvent(event);
}

TeamWidget::TeamWidget(GameController* controller, QWidget* parent)
    : QWidget(parent), game_controller(controller), main_layout(nullptr), title_label(nullptr) {
    setup_ui();
    
    connect(game_controller, &GameController::mode_changed, this, &TeamWidget::on_teams_updated);
    connect(game_controller, &GameController::team_changed, this, &TeamWidget::on_current_team_changed);
}

void TeamWidget::setup_ui() {
    main_layout = new QVBoxLayout(this);
    main_layout->setSpacing(10);
    main_layout->setContentsMargins(10, 10, 10, 10);
    
    title_label = new QLabel("Teams");
    title_label->setStyleSheet(
        "QLabel {"
        "font-size: 20px;"
        "font-weight: bold;"
        "color: #ffd700;"
        "padding: 10px;"
        "text-align: center;"
        "}"
    );
    title_label->setAlignment(Qt::AlignCenter);
    
    main_layout->addWidget(title_label);
    main_layout->addStretch();
}

void TeamWidget::clear_team_displays() {
    for (auto* display : team_displays) {
        main_layout->removeWidget(display);
        display->deleteLater();
    }
    team_displays.clear();
}

void TeamWidget::create_team_displays() {
    clear_team_displays();
    
    const auto& teams = game_controller->get_teams();
    const auto& current_team = game_controller->get_current_team();
    
    for (const auto& team : teams) {
        TeamDisplayWidget* display = new TeamDisplayWidget(&team, this);
        display->set_current_team(&team == &current_team);
        
        connect(display, &TeamDisplayWidget::name_change_requested,
                this, &TeamWidget::on_team_name_change_requested);
        
        team_displays.push_back(display);
        main_layout->insertWidget(main_layout->count() - 1, display);
    }
}

void TeamWidget::update_teams() {
    create_team_displays();
}

void TeamWidget::set_current_team(const team& current_team) {
    const auto& teams = game_controller->get_teams();
    for (size_t i = 0; i < team_displays.size() && i < teams.size(); ++i) {
        team_displays[i]->set_current_team(&teams[i] == &current_team);
    }
}

void TeamWidget::on_teams_updated() {
    update_teams();
}

void TeamWidget::on_current_team_changed(const team& current_team) {
    set_current_team(current_team);
    
    // Update all team displays to refresh scores
    for (auto* display : team_displays) {
        display->update_team_info();
    }
}

void TeamWidget::on_team_name_change_requested(const team* team_ptr, const QString& new_name) {
    // Find the team index
    const auto& teams = game_controller->get_teams();
    for (size_t i = 0; i < teams.size(); ++i) {
        if (&teams[i] == team_ptr) {
            game_controller->set_team_name(i, new_name.toStdString());
            break;
        }
    }
}
