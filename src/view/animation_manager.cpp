#include "view/animation_manager.h"
#include <QEasingCurve>
#include <QRect>
#include <QDebug>

AnimationManager::AnimationManager(QWidget* parent_widget, QObject* parent)
    : QObject(parent), parent_widget(parent_widget), correct_sequence(nullptr), 
      incorrect_sequence(nullptr), animation_feedback_label(nullptr), 
      animation_completion_timer(nullptr), animation_in_progress(false) {
    setup_animations();
}

AnimationManager::~AnimationManager() {
    cleanup_animations();
}

void AnimationManager::setup_animations() {
    animation_completion_timer = new QTimer(this);
    animation_completion_timer->setSingleShot(true);
    animation_feedback_label = new QLabel(parent_widget);
    animation_feedback_label->setAlignment(Qt::AlignCenter);
    animation_feedback_label->setStyleSheet(
        "QLabel { background-color: rgba(0, 0, 0, 0.7); color: white; "
        "font-size: 28px; font-weight: bold; border-radius: 10px; padding: 20px; }"
    );
    animation_feedback_label->setVisible(false);
    animation_feedback_label->setGeometry(0, 0, parent_widget->width(), parent_widget->height());
    animation_in_progress = false;
    correct_sequence = nullptr;
    incorrect_sequence = nullptr;
}

void AnimationManager::cleanup_animations() {
    if (correct_sequence) {
        correct_sequence->stop();
        correct_sequence->deleteLater();
        correct_sequence = nullptr;
    }
    
    if (incorrect_sequence) {
        incorrect_sequence->stop();
        incorrect_sequence->deleteLater();
        incorrect_sequence = nullptr;
    }
    
    if (animation_completion_timer) {
        animation_completion_timer->stop();
        animation_completion_timer->disconnect();
    }
    
    cleanup_leftover_labels();
    
    // Clean up any running property animations that might still be active
    QList<QPropertyAnimation*> running_animations = parent_widget->findChildren<QPropertyAnimation*>();
    for (QPropertyAnimation* anim : running_animations) {
        if (anim->state() == QAbstractAnimation::Running) {
            anim->stop();
            anim->deleteLater();
        }
    }
    
    // Clean up any animation groups that might still be active
    QList<QAnimationGroup*> running_groups = parent_widget->findChildren<QAnimationGroup*>();
    for (QAnimationGroup* group : running_groups) {
        if (group->state() == QAbstractAnimation::Running) {
            group->stop();
            group->deleteLater();
        }
    }
    
    // Reset animation state
    animation_in_progress = false;
}

void AnimationManager::play_correct_animation(int points) {
    if (animation_in_progress) return;
    
    animation_in_progress = true;
    setup_timeout_protection(true);
    create_correct_animation_sequence(points);
}

void AnimationManager::play_incorrect_animation(int points) {
    if (animation_in_progress) return;
    
    animation_in_progress = true;
    setup_timeout_protection(false);
    create_incorrect_animation_sequence(points);
}

void AnimationManager::create_correct_animation_sequence(int points) {
    if (correct_sequence) {
        correct_sequence->stop();
        correct_sequence->deleteLater();
    }
    
    correct_sequence = new QSequentialAnimationGroup(this);
    
    animation_feedback_label->setText("CORRECT!");
    animation_feedback_label->setStyleSheet(
        "QLabel { background-color: rgba(76, 175, 80, 0.9); color: white; "
        "font-size: 32px; font-weight: bold; border-radius: 15px; padding: 30px; }"
    );
    animation_feedback_label->setGeometry(50, 100, parent_widget->width() - 100, 120);
    animation_feedback_label->setVisible(true);
    
    // Create fade-in animation for overlay
    QPropertyAnimation* overlay_fade_in = new QPropertyAnimation(animation_feedback_label, "windowOpacity");
    overlay_fade_in->setDuration(OVERLAY_FADE_DURATION);
    overlay_fade_in->setStartValue(0.0);
    overlay_fade_in->setEndValue(1.0);
    overlay_fade_in->setEasingCurve(QEasingCurve::OutQuad);
    
    // Create points animation
    create_points_animation(points);
    
    // Create fade-out animation for overlay
    QPropertyAnimation* overlay_fade_out = new QPropertyAnimation(animation_feedback_label, "windowOpacity");
    overlay_fade_out->setDuration(OVERLAY_FADE_DURATION);
    overlay_fade_out->setStartValue(1.0);
    overlay_fade_out->setEndValue(0.0);
    overlay_fade_out->setEasingCurve(QEasingCurve::InQuad);
    
    // Add animations to sequence
    correct_sequence->addAnimation(overlay_fade_in);
    correct_sequence->addAnimation(overlay_fade_out);
    
    // Connect completion signal
    connect(correct_sequence, &QSequentialAnimationGroup::finished, 
            this, &AnimationManager::on_correct_sequence_finished);
    
    // Start the animation
    correct_sequence->start();
}

void AnimationManager::create_incorrect_animation_sequence(int points) {
    if (incorrect_sequence) {
        incorrect_sequence->stop();
        incorrect_sequence->deleteLater();
    }
    
    incorrect_sequence = new QSequentialAnimationGroup(this);
    
    animation_feedback_label->setText("INCORRECT!");
    animation_feedback_label->setStyleSheet(
        "QLabel { background-color: rgba(244, 67, 54, 0.9); color: white; "
        "font-size: 32px; font-weight: bold; border-radius: 15px; padding: 30px; }"
    );
    animation_feedback_label->setGeometry(50, 100, parent_widget->width() - 100, 120);
    animation_feedback_label->setVisible(true);
    
    // Create fade-in animation for overlay
    QPropertyAnimation* overlay_fade_in = new QPropertyAnimation(animation_feedback_label, "windowOpacity");
    overlay_fade_in->setDuration(OVERLAY_FADE_DURATION);
    overlay_fade_in->setStartValue(0.0);
    overlay_fade_in->setEndValue(1.0);
    overlay_fade_in->setEasingCurve(QEasingCurve::OutQuad);
    
    // Create shake animation for the dialog
    create_shake_animation();
    
    // Create points deduction animation
    QLabel* deduction_label = new QLabel(parent_widget);
    deduction_label->setStyleSheet(
        "QLabel { color: #f44336; font-size: 36px; font-weight: bold; "
        "background-color: rgba(0, 0, 0, 0.8); border-radius: 10px; padding: 15px; }"
    );
    deduction_label->setAlignment(Qt::AlignCenter);
    deduction_label->setGeometry(parent_widget->width() / 2 - 100, parent_widget->height() / 2 + 50, 200, 100);
    deduction_label->setText(QString("-$%1").arg(points));
    deduction_label->setVisible(true);
    
    // Create fade-out animation for overlay
    QPropertyAnimation* overlay_fade_out = new QPropertyAnimation(animation_feedback_label, "windowOpacity");
    overlay_fade_out->setDuration(OVERLAY_FADE_DURATION);
    overlay_fade_out->setStartValue(1.0);
    overlay_fade_out->setEndValue(0.0);
    overlay_fade_out->setEasingCurve(QEasingCurve::InQuad);
    
    // Add animations to sequence
    incorrect_sequence->addAnimation(overlay_fade_in);
    incorrect_sequence->addAnimation(overlay_fade_out);
    
    // Clean up the deduction label when animation finishes
    connect(incorrect_sequence, &QSequentialAnimationGroup::finished, [deduction_label]() {
        deduction_label->deleteLater();
    });
    
    // Connect completion signal
    connect(incorrect_sequence, &QSequentialAnimationGroup::finished, 
            this, &AnimationManager::on_incorrect_sequence_finished);
    
    // Start the animation
    incorrect_sequence->start();
}

void AnimationManager::create_points_animation(int points) {
    QLabel* points_animation_label = new QLabel(parent_widget);
    points_animation_label->setObjectName("points_animation_label");
    points_animation_label->setStyleSheet(
        "QLabel { color: #4caf50; font-size: 36px; font-weight: bold; "
        "background-color: rgba(0, 0, 0, 0.8); border-radius: 10px; padding: 15px; }"
    );
    points_animation_label->setAlignment(Qt::AlignCenter);
    points_animation_label->setGeometry(parent_widget->width() / 2 - 100, parent_widget->height() / 2 - 50, 200, 100);
    points_animation_label->setText(QString("+$%1").arg(points));
    points_animation_label->setVisible(true);
    
    // Create animations for the points
    QPropertyAnimation* points_fade_in = new QPropertyAnimation(points_animation_label, "windowOpacity");
    points_fade_in->setDuration(POINTS_COUNT_DURATION / 3);
    points_fade_in->setStartValue(0.0);
    points_fade_in->setEndValue(1.0);
    points_fade_in->setEasingCurve(QEasingCurve::OutQuad);
    
    QPropertyAnimation* points_move = new QPropertyAnimation(points_animation_label, "geometry");
    points_move->setDuration(POINTS_COUNT_DURATION);
    points_move->setStartValue(points_animation_label->geometry());
    QRect final_rect = points_animation_label->geometry();
    final_rect.moveTop(final_rect.top() - 50);
    points_move->setEndValue(final_rect);
    points_move->setEasingCurve(QEasingCurve::OutQuad);
    
    QPropertyAnimation* points_fade_out = new QPropertyAnimation(points_animation_label, "windowOpacity");
    points_fade_out->setDuration(POINTS_COUNT_DURATION / 3);
    points_fade_out->setStartValue(1.0);
    points_fade_out->setEndValue(0.0);
    points_fade_out->setEasingCurve(QEasingCurve::InQuad);
    
    QParallelAnimationGroup* points_group = new QParallelAnimationGroup(this);
    points_group->addAnimation(points_fade_in);
    points_group->addAnimation(points_move);
    points_group->addAnimation(points_fade_out);
    
    // Clean up the temporary label when animation finishes
    connect(points_group, &QParallelAnimationGroup::finished, [points_animation_label, points_group]() {
        points_animation_label->hide();
        points_animation_label->deleteLater();
        points_group->deleteLater();
    });
    
    points_group->start();
}

void AnimationManager::create_shake_animation() {
    QRect original_geometry = parent_widget->geometry();
    
    QPropertyAnimation* shake_animation = new QPropertyAnimation(parent_widget, "geometry");
    shake_animation->setDuration(SHAKE_DURATION);
    shake_animation->setEasingCurve(QEasingCurve::InOutQuad);
    
    // Define shake positions
    QRect shake_left = original_geometry;
    shake_left.moveLeft(original_geometry.left() - 10);
    
    QRect shake_right = original_geometry;
    shake_right.moveLeft(original_geometry.left() + 10);
    
    // Create keyframe values for shake effect
    shake_animation->setKeyValueAt(0.0, original_geometry);
    shake_animation->setKeyValueAt(0.1, shake_left);
    shake_animation->setKeyValueAt(0.2, shake_right);
    shake_animation->setKeyValueAt(0.3, shake_left);
    shake_animation->setKeyValueAt(0.4, shake_right);
    shake_animation->setKeyValueAt(0.5, shake_left);
    shake_animation->setKeyValueAt(0.6, shake_right);
    shake_animation->setKeyValueAt(0.7, shake_left);
    shake_animation->setKeyValueAt(0.8, shake_right);
    shake_animation->setKeyValueAt(0.9, shake_left);
    shake_animation->setKeyValueAt(1.0, original_geometry);
    
    shake_animation->start();
    
    connect(shake_animation, &QPropertyAnimation::finished, [shake_animation]() {
        shake_animation->deleteLater();
    });
}

void AnimationManager::setup_timeout_protection(bool is_correct) {
    animation_completion_timer->stop();
    animation_completion_timer->disconnect();
    
    if (is_correct) {
        connect(animation_completion_timer, &QTimer::timeout, 
                this, &AnimationManager::on_correct_animation_timeout);
        animation_completion_timer->start(TOTAL_CORRECT_DURATION + 500);
    } else {
        connect(animation_completion_timer, &QTimer::timeout, 
                this, &AnimationManager::on_incorrect_animation_timeout);
        animation_completion_timer->start(TOTAL_INCORRECT_DURATION + 500);
    }
}

void AnimationManager::cleanup_leftover_labels() {
    QList<QLabel*> leftover_labels = parent_widget->findChildren<QLabel*>();
    for (QLabel* label : leftover_labels) {
        if (label->objectName() == "points_animation_label" || 
            label->text().startsWith("+$") || label->text().startsWith("-$")) {
            label->hide();
            label->deleteLater();
        }
    }
}

void AnimationManager::on_correct_animation_timeout() {
    if (animation_in_progress) {
        qWarning("Correct animation timeout - forcing completion");
        on_correct_sequence_finished();
    }
}

void AnimationManager::on_incorrect_animation_timeout() {
    if (animation_in_progress) {
        qWarning("Incorrect animation timeout - forcing completion");
        on_incorrect_sequence_finished();
    }
}

void AnimationManager::on_correct_sequence_finished() {
    animation_in_progress = false;
    
    if (animation_completion_timer) {
        animation_completion_timer->stop();
    }
    
    if (animation_feedback_label) {
        animation_feedback_label->setVisible(false);
    }
    
    emit correct_animation_finished();
}

void AnimationManager::on_incorrect_sequence_finished() {
    animation_in_progress = false;
    
    if (animation_completion_timer) {
        animation_completion_timer->stop();
    }
    
    if (animation_feedback_label) {
        animation_feedback_label->setVisible(false);
    }
    
    emit incorrect_animation_finished();
}

void AnimationManager::set_buttons_enabled(bool enabled) {
    // This will be called from the QuestionDialog to enable/disable buttons
    // The actual button references will be handled by the dialog
}
