#include "pomodoro.h"
#include "ui_pomodoro.h"

#include <QObject>
#include <QString>
#include <QTime>
#include <QSound>
#include <QShortcut>

Pomodoro::Pomodoro(QWidget *parent)
    : QMainWindow(parent)
    , m_timer{new QTimer(this)}
    , m_pause_shortcut{new QShortcut(Qt::Key_Space, this)}
#ifdef Q_OS_WIN
    , m_taskbar_button{new QWinTaskbarButton(this)}
    , m_taskbar_progress{m_taskbar_button->progress()}
    , m_thumbbar{new QWinThumbnailToolBar(this)}
    , m_toggle_toolbar_button{new QWinThumbnailToolButton(m_thumbbar)}
#endif
    , ui(new Ui::Pomodoro)
{
    ui->setupUi(this);
    this->setFixedSize(260, 160);
    ui->label->setFont(QFont(":/fonts/res/fonts/IBMPlexMono-Bold.ttf", 50));

    QObject::connect(m_timer.get(), &QTimer::timeout, this, &Pomodoro::tick);
    QObject::connect(ui->goButton, &QPushButton::clicked, this, &Pomodoro::toggle);
    QObject::connect(ui->resetButton, &QPushButton::clicked, this, &Pomodoro::reset);
    QObject::connect(ui->actionAbout, &QAction::triggered, this, &Pomodoro::show_about);

    QObject::connect(m_pause_shortcut, &QShortcut::activated, this, &Pomodoro::toggle);
    QObject::connect(ui->actionAlways_On_Top, &QAction::toggled, this, &Pomodoro::always_on_top_trigger);

    m_timer->start(1000);

    reset();

#ifdef Q_OS_WIN
    m_taskbar_progress->show();
    m_taskbar_progress->resume();
#endif

}


Pomodoro::~Pomodoro()
{
    delete ui;
}

void Pomodoro::showEvent(QShowEvent *event) {
#ifdef Q_OS_WIN32
    m_taskbar_button->setWindow(windowHandle());

    // ------------- Create buttons ----------------------
    m_thumbbar->setWindow(windowHandle());
    QObject::connect(m_toggle_toolbar_button, &QWinThumbnailToolButton::clicked, this, &Pomodoro::toggle);

    m_thumbbar->addButton(m_toggle_toolbar_button);
#endif

    event->accept();
}

void Pomodoro::always_on_top_trigger(bool checked) {
#ifdef Q_OS_WIN
    #include <windows.h>
    HWND win_id = reinterpret_cast<HWND>(this->winId());
    if (checked)
    {
        SetWindowPos(win_id, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    else
    {
        SetWindowPos(win_id, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
#else
    Qt::WindowFlags flags = this->windowFlags();
    if (checked)
    {
        this->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
        this->show();
    }
    else
    {
        this->setWindowFlags(flags ^ (Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
        this->show();
    }
#endif
}

void Pomodoro::test_slot() {
    QMessageBox::information(this, "Title", "Hello world");
}

void Pomodoro::show_about() {
    QFile file(":/text/res/text/about.txt");
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, "About", "Cannot open about.txt information.");
    } else {
        QString content = file.readAll();
        QMessageBox::about(this, "About", content);
    }
}


void Pomodoro::tick() {
    if (!m_paused) {
        update();
    }
}

void Pomodoro::update() {
    set_remaining(m_remaining - 1);

    if (m_remaining == 0) {
        QSound::play(":/sounds/res/sounds/bell.wav");
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
#ifdef Q_OS_WIN
    m_taskbar_progress->setValue(m_total - x);
#endif

    auto time = QTime(0, 0, 0, 0).addSecs(x);
    QString display = time.toString("mm:ss");
    ui->label->setText(display);
}

void Pomodoro::set_counter(int x) {
    m_counter = x;
    ui->status_label->setText(QString("Sessions: %1").arg(QString::number(m_counter)));
}

void Pomodoro::reset_time(int duration) {
    set_remaining(duration);
    m_total = duration;
#ifdef Q_OS_WIN
    m_taskbar_progress->setRange(0, duration);
    m_taskbar_progress->setValue(0);
#endif

}

void Pomodoro::start_short_break() {
    reset_time(short_break_duration);
    m_working = false;
    ui->label->setStyleSheet(
    QStringLiteral("QLabel{ background: rgb( 35, 98,103); color: rgb(207,226,227);}")
    );

}

void Pomodoro::start_long_break() {
    reset_time(long_break_duration);
    m_working = false;

    ui->label->setStyleSheet(
    QStringLiteral("QLabel{ background: rgb(120,158, 53); color: rgb(242,250,227);}")
    );
}

void Pomodoro::start_work() {
    reset_time(work_duration);
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

    auto icon = QIcon(":/icons/res/icons/control_play_blue.png");
    ui->goButton->setIcon(icon);

#ifdef Q_OS_WIN
    m_toggle_toolbar_button->setIcon(icon);
    m_taskbar_progress->pause();
#endif
}

void Pomodoro::go() {
    m_paused = false;
    ui->goButton->setText(QStringLiteral("Pause"));

    auto icon = QIcon(":/icons/res/icons/control_pause_blue.png");
    ui->goButton->setIcon(icon);

#ifdef Q_OS_WIN
    m_toggle_toolbar_button->setIcon(icon);
    m_taskbar_progress->resume();
#endif
}

void Pomodoro::toggle() {
    if (m_paused) {
        go();
    } else {
        pause();
    }
}
