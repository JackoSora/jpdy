#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFrame>
#include <QString>
#include <QEvent>
#include <QKeyEvent>
#include <vector>
#include "model/team.h"

class GameController;

class TeamDisplayWidget : public QFrame {
    Q_OBJECT

private:
    const team* team_data;
    QLabel* name_label;
    QLabel* score_label;
    QLineEdit* name_edit;
    bool is_current_team;
    bool is_editing_name;

    void setup_ui();
    void update_display();
    void start_name_editing();
    void finish_name_editing();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

public:
    explicit TeamDisplayWidget(const team* team_ptr, QWidget* parent = nullptr);
    void set_team_data(const team* team_ptr);
    void set_current_team(bool is_current);
    void update_team_info();

signals:
    void name_change_requested(const team* team_ptr, const QString& new_name);

private slots:
    void on_name_clicked();
    void on_name_edit_finished();
    void on_name_edit_escape();
};

class TeamWidget : public QWidget {
    Q_OBJECT

private:
    GameController* game_controller;
    QVBoxLayout* main_layout;
    QLabel* title_label;
    std::vector<TeamDisplayWidget*> team_displays;

    void setup_ui();
    void clear_team_displays();
    void create_team_displays();

public:
    explicit TeamWidget(GameController* controller, QWidget* parent = nullptr);
    void update_teams();
    void set_current_team(const team& current_team);

public slots:
    void on_teams_updated();
    void on_current_team_changed(const team& current_team);

private slots:
    void on_team_name_change_requested(const team* team_ptr, const QString& new_name);
};
