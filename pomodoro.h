#ifndef POMODORO_H
#define POMODORO_H

#include <QMainWindow>
#include <QTimer>
#include <QtCore>
#include <QtWidgets>


#ifdef Q_OS_WIN
#include <QWinTaskbarProgress>
#include <QWinTaskbarButton>
#include <QWinThumbnailToolBar>
#include <QWinThumbnailToolButton>
#endif

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class Pomodoro; }
QT_END_NAMESPACE

class Pomodoro : public QMainWindow
{
    Q_OBJECT

public:
    Pomodoro(QWidget *parent = nullptr);
    ~Pomodoro();

    const int long_break_duration = 15 * 60;
    const int short_break_duration = 5 * 60;
    const int work_duration = 25 * 60;

    bool m_paused;

    int m_remaining;
    int m_total;
    int m_counter;
    bool m_working;
    bool m_always_on_top;

    std::unique_ptr<QTimer> m_timer;
    QShortcut* m_pause_shortcut;

#ifdef Q_OS_WIN
    QWinTaskbarButton* m_taskbar_button;
    QWinTaskbarProgress* m_taskbar_progress;

    QWinThumbnailToolBar* m_thumbbar;
    QWinThumbnailToolButton* m_toggle_toolbar_button;
#endif

private slots:
    void tick();

private:
    void showEvent(QShowEvent*);
    void always_on_top_trigger(bool);

    void update();
    void reset_time(int);
    void start_short_break();
    void start_long_break();
    void start_work();

    void reset();
    void pause();
    void go();
    void toggle();

    void set_remaining(int);
    void set_counter(int);

    void show_about();

    void test_slot();

    Ui::Pomodoro *ui;
};
#endif // POMODORO_H
