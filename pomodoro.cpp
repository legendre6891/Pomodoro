#include "pomodoro.h"
#include "ui_pomodoro.h"

#include <QtCore>
#include <QtWidgets>
#include <QObject>
#include <QString>
#include <QTime>

#include <iostream>

Pomodoro::Pomodoro(QWidget *parent)
    : QMainWindow(parent)
    , m_timer{new QTimer(this)}
    , ui(new Ui::Pomodoro)
{
    ui->setupUi(this);
    ui->resetButton->setIcon(QIcon(":/icons/res/icons/arrow_refresh.png"));
    QObject::connect(m_timer.get(), &QTimer::timeout, this, &Pomodoro::tick);
    QObject::connect(ui->goButton, &QPushButton::clicked, this, &Pomodoro::toggle);
    QObject::connect(ui->resetButton, &QPushButton::clicked, this, &Pomodoro::reset);

    m_timer->start(5);
    reset();
}

Pomodoro::~Pomodoro()
{
    delete ui;
}

void Pomodoro::tick() {
    if (!m_paused) {
        update();
    }
}

void Pomodoro::update() {
    set_remaining(m_remaining - 1);

    if (m_remaining == 0) {
        if (m_working == true) {
            set_counter(m_counter + 1);
            if (m_counter % 4 == 0) {
                start_long_break();
            } else {
                start_short_break();
            }
        } else {
            start_work();
        }
    }
}

void Pomodoro::set_remaining(int x) {
    m_remaining = x;
    auto time = QTime(0, 0, 0, 0).addSecs(x);

    QString display = time.toString("mm:ss");
    ui->label->setText(display);
}

void Pomodoro::set_counter(int x) {
    m_counter = x;
    ui->status_label->setText(QString("Sessions: %1").arg(QString::number(m_counter)));
}

void Pomodoro::start_short_break() {
    set_remaining(short_break_duration);
    m_working = false;

    ui->label->setStyleSheet(
    QStringLiteral("QLabel{ background: rgb( 35, 98,103); color: rgb(207,226,227);}")
    );

}

void Pomodoro::start_long_break() {
    set_remaining(long_break_duration);
    m_working = false;

    ui->label->setStyleSheet(
    QStringLiteral("QLabel{ background: rgb(120,158, 53); color: rgb(242,250,227);}")
    );
}

void Pomodoro::start_work() {
    set_remaining(work_duration);
    m_working = true;

    ui->label->setStyleSheet(
    QStringLiteral("QLabel{ background: rgb(170, 62, 57); color: rgb(255,233,232);}")
    );
}

void Pomodoro::reset() {
    pause();
    set_counter(0);
    start_work();
}

void Pomodoro::pause() {
    m_paused = true;
    ui->goButton->setText(QStringLiteral("Resume"));
    ui->goButton->setIcon(QIcon(":/icons/res/icons/control_play_blue.png"));
}

void Pomodoro::go() {
    m_paused = false;
    ui->goButton->setText(QStringLiteral("Pause"));
    ui->goButton->setIcon(QIcon(":/icons/res/icons/control_pause_blue.png"));
}

void Pomodoro::toggle() {
    if (m_paused) {
        go();
    } else {
        pause();
    }
}
