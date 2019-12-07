#ifndef POMODORO_H
#define POMODORO_H

#include <QMainWindow>
#include <QTimer>
#include <QtCore>
#include <QtWidgets>
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
    int m_counter;
    bool m_working;

    std::unique_ptr<QTimer> m_timer;


private slots:
    void tick();

private:
    void update();
    void start_short_break();
    void start_long_break();
    void start_work();

    void reset();
    void pause();
    void go();
    void toggle();

    void set_remaining(int);
    void set_counter(int);

    Ui::Pomodoro *ui;
};
#endif // POMODORO_H
