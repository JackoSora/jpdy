#pragma once
#include <QObject>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QLabel>
#include <QWidget>

class AnimationManager : public QObject {
    Q_OBJECT

public:
    explicit AnimationManager(QWidget* parent_widget, QObject* parent = nullptr);
    ~AnimationManager();

    void setup_animations();
    void cleanup_animations();
    
    void play_correct_animation(int points);
    void play_incorrect_animation(int points);
    
    bool is_animation_in_progress() const { return animation_in_progress; }
    void set_buttons_enabled(bool enabled);

signals:
    void correct_animation_finished();
    void incorrect_animation_finished();

private slots:
    void on_correct_animation_timeout();
    void on_incorrect_animation_timeout();
    void on_correct_sequence_finished();
    void on_incorrect_sequence_finished();

private:
    // Animation timing constants
    static constexpr int OVERLAY_FADE_DURATION = 300;
    static constexpr int POINTS_COUNT_DURATION = 800;
    static constexpr int SHAKE_DURATION = 400;
    static constexpr int DIALOG_FADE_DURATION = 500;
    static constexpr int TOTAL_CORRECT_DURATION = 1200;
    static constexpr int TOTAL_INCORRECT_DURATION = 900;

    // Parent widget reference
    QWidget* parent_widget;
    
    // Animation infrastructure
    QSequentialAnimationGroup* correct_sequence;
    QSequentialAnimationGroup* incorrect_sequence;
    QLabel* animation_feedback_label;
    QTimer* animation_completion_timer;
    bool animation_in_progress;
    
    // Animation creation methods
    void create_correct_animation_sequence(int points);
    void create_incorrect_animation_sequence(int points);
    void create_points_animation(int points);
    void create_shake_animation();
    void create_overlay_animation(const QString& text, const QString& color, bool fade_in);
    
    // Helper methods
    void cleanup_leftover_labels();
    void setup_timeout_protection(bool is_correct);
};
