#include "view/dialog_transition_manager.h"
#include <QEasingCurve>

DialogTransitionManager::DialogTransitionManager(QDialog* dialog, QObject* parent)
    : QObject(parent), dialog(dialog), dialog_opacity_effect(nullptr), fade_out_animation(nullptr) {
    setup_transition_effects();
}

DialogTransitionManager::~DialogTransitionManager() {
    if (fade_out_animation) {
        fade_out_animation->stop();
        fade_out_animation->deleteLater();
    }
}

void DialogTransitionManager::setup_transition_effects() {
    // Create graphics opacity effect for dialog fade
    dialog_opacity_effect = new QGraphicsOpacityEffect(dialog);
    dialog_opacity_effect->setOpacity(1.0);
    dialog->setGraphicsEffect(dialog_opacity_effect);
}

void DialogTransitionManager::fade_out_and_close() {
    if (fade_out_animation) {
        fade_out_animation->stop();
        fade_out_animation->deleteLater();
    }
    
    // Create fade-out animation for smooth dialog transition
    fade_out_animation = new QPropertyAnimation(dialog_opacity_effect, "opacity", this);
    fade_out_animation->setDuration(DIALOG_FADE_DURATION);
    fade_out_animation->setStartValue(1.0);
    fade_out_animation->setEndValue(0.0);
    fade_out_animation->setEasingCurve(QEasingCurve::InQuad);
    
    // Connect completion signal
    connect(fade_out_animation, &QPropertyAnimation::finished, 
            this, &DialogTransitionManager::on_fade_out_finished);
    
    fade_out_animation->start();
}

void DialogTransitionManager::reset_opacity() {
    if (dialog_opacity_effect) {
        dialog_opacity_effect->setOpacity(1.0);
    }
}

void DialogTransitionManager::on_fade_out_finished() {
    if (fade_out_animation) {
        fade_out_animation->deleteLater();
        fade_out_animation = nullptr;
    }
    
    emit transition_finished();
}
