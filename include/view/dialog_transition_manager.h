#pragma once
#include <QObject>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QDialog>

class DialogTransitionManager : public QObject {
    Q_OBJECT

public:
    explicit DialogTransitionManager(QDialog* dialog, QObject* parent = nullptr);
    ~DialogTransitionManager();

    void setup_transition_effects();
    void fade_out_and_close();
    void reset_opacity();

signals:
    void transition_finished();

private slots:
    void on_fade_out_finished();

private:
    static constexpr int DIALOG_FADE_DURATION = 500;
    
    QDialog* dialog;
    QGraphicsOpacityEffect* dialog_opacity_effect;
    QPropertyAnimation* fade_out_animation;
};
