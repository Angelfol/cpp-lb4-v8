#include "MainWindow.h"

#include <QApplication>
#include <QElapsedTimer>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLCDNumber>
#include <QListWidget>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QTabWidget>
#include <QTime>
#include <QTimeEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QtGlobal>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
}

void MainWindow::setupUi()
{
    baseWindowTitle = QStringLiteral("ЛР4 - Таймер и секундомер (Qt Widgets)");
    setWindowTitle(baseWindowTitle);
    resize(880, 640);

    auto *central = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(central);

    tabWidget = new QTabWidget(central);
    tabWidget->addTab(createStopwatchTab(), QStringLiteral("Секундомер"));
    tabWidget->addTab(createTimerTab(), QStringLiteral("Таймер"));
    mainLayout->addWidget(tabWidget);

    usageStatsLabel = new QLabel(central);
    mainLayout->addWidget(usageStatsLabel);
    refreshUsageStats();

    setCentralWidget(central);
}

QWidget *MainWindow::createStopwatchTab()
{
    auto *tab = new QWidget(this);
    auto *layout = new QVBoxLayout(tab);

    stopwatchDisplay = new QLabel(QStringLiteral("00:00:00.000"), tab);
    stopwatchDisplay->setAlignment(Qt::AlignCenter);
    stopwatchDisplay->setStyleSheet(
        "font-size: 42px;"
        "font-weight: 700;"
        "padding: 14px;"
        "background-color: #f0f6ff;"
        "border: 2px solid #9cb2d7;"
        "border-radius: 8px;");
    layout->addWidget(stopwatchDisplay);

    auto *buttonRow = new QHBoxLayout();
    stopwatchStartButton = new QPushButton(QStringLiteral("Старт"), tab);
    stopwatchStopButton = new QPushButton(QStringLiteral("Стоп"), tab);
    stopwatchResetButton = new QPushButton(QStringLiteral("Сброс"), tab);
    stopwatchLapButton = new QPushButton(QStringLiteral("Круг"), tab);

    buttonRow->addWidget(stopwatchStartButton);
    buttonRow->addWidget(stopwatchStopButton);
    buttonRow->addWidget(stopwatchResetButton);
    buttonRow->addWidget(stopwatchLapButton);
    layout->addLayout(buttonRow);

    auto *lapsGroup = new QGroupBox(QStringLiteral("Промежуточные результаты"), tab);
    auto *lapsLayout = new QVBoxLayout(lapsGroup);
    lapList = new QListWidget(lapsGroup);
    lapsLayout->addWidget(lapList);
    layout->addWidget(lapsGroup);

    stopwatchTimer = new QTimer(this);
    stopwatchTimer->setInterval(10);

    connect(stopwatchStartButton, &QPushButton::clicked, this, &MainWindow::startStopwatch);
    connect(stopwatchStopButton, &QPushButton::clicked, this, &MainWindow::stopStopwatch);
    connect(stopwatchResetButton, &QPushButton::clicked, this, &MainWindow::resetStopwatch);
    connect(stopwatchLapButton, &QPushButton::clicked, this, &MainWindow::saveLap);
    connect(stopwatchTimer, &QTimer::timeout, this, &MainWindow::updateStopwatchDisplay);

    return tab;
}

QWidget *MainWindow::createTimerTab()
{
    auto *tab = new QWidget(this);
    auto *layout = new QVBoxLayout(tab);

    auto *inputGroup = new QGroupBox(QStringLiteral("Настройка таймера"), tab);
    auto *inputLayout = new QGridLayout(inputGroup);

    auto *inputLabel = new QLabel(QStringLiteral("Время (чч:мм:сс):"), inputGroup);
    timerInput = new QTimeEdit(QTime(0, 25, 0), inputGroup);
    timerInput->setDisplayFormat(QStringLiteral("HH:mm:ss"));
    timerInput->setMinimumTime(QTime(0, 0, 1));
    timerInput->setMaximumTime(QTime(23, 59, 59));

    inputLayout->addWidget(inputLabel, 0, 0);
    inputLayout->addWidget(timerInput, 0, 1);
    layout->addWidget(inputGroup);

    auto *presetGroup = new QGroupBox(QStringLiteral("Быстрые пресеты"), tab);
    auto *presetLayout = new QHBoxLayout(presetGroup);
    auto *preset5 = new QPushButton(QStringLiteral("5 мин"), presetGroup);
    auto *preset10 = new QPushButton(QStringLiteral("10 мин"), presetGroup);
    auto *preset25 = new QPushButton(QStringLiteral("25 мин (Pomodoro)"), presetGroup);
    presetLayout->addWidget(preset5);
    presetLayout->addWidget(preset10);
    presetLayout->addWidget(preset25);
    layout->addWidget(presetGroup);

    countdownDisplay = new QLCDNumber(tab);
    countdownDisplay->setDigitCount(8);
    countdownDisplay->setSegmentStyle(QLCDNumber::Filled);
    countdownDisplay->display(QStringLiteral("00:25:00"));
    layout->addWidget(countdownDisplay);

    countdownProgress = new QProgressBar(tab);
    countdownProgress->setRange(0, 100);
    countdownProgress->setValue(0);
    countdownProgress->setFormat(QStringLiteral("Прогресс: %p%"));
    layout->addWidget(countdownProgress);

    auto *buttonRow = new QHBoxLayout();
    countdownStartButton = new QPushButton(QStringLiteral("Старт"), tab);
    countdownPauseButton = new QPushButton(QStringLiteral("Пауза"), tab);
    countdownResetButton = new QPushButton(QStringLiteral("Сброс"), tab);
    buttonRow->addWidget(countdownStartButton);
    buttonRow->addWidget(countdownPauseButton);
    buttonRow->addWidget(countdownResetButton);
    layout->addLayout(buttonRow);

    countdownTimer = new QTimer(this);
    countdownTimer->setInterval(100);

    blinkTimer = new QTimer(this);
    blinkTimer->setInterval(250);

    connect(countdownStartButton, &QPushButton::clicked, this, &MainWindow::startCountdown);
    connect(countdownPauseButton, &QPushButton::clicked, this, &MainWindow::pauseCountdown);
    connect(countdownResetButton, &QPushButton::clicked, this, &MainWindow::resetCountdown);
    connect(countdownTimer, &QTimer::timeout, this, &MainWindow::updateCountdown);
    connect(timerInput, &QTimeEdit::timeChanged, this, &MainWindow::onTimerInputChanged);
    connect(preset5, &QPushButton::clicked, this, &MainWindow::onPreset5Minutes);
    connect(preset10, &QPushButton::clicked, this, &MainWindow::onPreset10Minutes);
    connect(preset25, &QPushButton::clicked, this, &MainWindow::onPreset25Minutes);
    connect(blinkTimer, &QTimer::timeout, this, &MainWindow::blinkCompletionState);

    onTimerInputChanged();
    return tab;
}

void MainWindow::startStopwatch()
{
    if (stopwatchRunning) {
        return;
    }

    stopwatchElapsed.restart();
    stopwatchRunning = true;
    stopwatchTimer->start();
    ++stopwatchStartCount;
    refreshUsageStats();
}

void MainWindow::stopStopwatch()
{
    if (!stopwatchRunning) {
        return;
    }

    stopwatchAccumulatedMs += stopwatchElapsed.elapsed();
    stopwatchRunning = false;
    stopwatchTimer->stop();
    updateStopwatchDisplay();
}

void MainWindow::resetStopwatch()
{
    stopStopwatch();
    stopwatchAccumulatedMs = 0;
    lapCounter = 0;
    lapList->clear();
    updateStopwatchDisplay();
}

void MainWindow::saveLap()
{
    qint64 totalMs = stopwatchAccumulatedMs;
    if (stopwatchRunning) {
        totalMs += stopwatchElapsed.elapsed();
    }

    if (totalMs == 0) {
        QMessageBox::information(this, QStringLiteral("Секундомер"), QStringLiteral("Сначала запустите секундомер."));
        return;
    }

    ++lapCounter;
    lapList->addItem(QStringLiteral("Круг %1: %2").arg(lapCounter).arg(formatStopwatchTime(totalMs)));
    lapList->scrollToBottom();
}

void MainWindow::updateStopwatchDisplay()
{
    qint64 totalMs = stopwatchAccumulatedMs;
    if (stopwatchRunning) {
        totalMs += stopwatchElapsed.elapsed();
    }
    stopwatchDisplay->setText(formatStopwatchTime(totalMs));
}

void MainWindow::startCountdown()
{
    if (countdownRunning) {
        return;
    }

    if (blinkTimer->isActive()) {
        blinkTimer->stop();
        setWindowTitle(baseWindowTitle);
    }

    if (countdownRemainingMs <= 0) {
        countdownTotalMs = timerInputToMilliseconds();
        countdownRemainingMs = countdownTotalMs;
    }

    if (countdownRemainingMs <= 0) {
        QMessageBox::warning(this, QStringLiteral("Ошибка ввода"), QStringLiteral("Установите время больше 00:00:00."));
        return;
    }

    if (countdownTotalMs <= 0) {
        countdownTotalMs = countdownRemainingMs;
    }

    countdownRunning = true;
    countdownTimer->start();
}

void MainWindow::pauseCountdown()
{
    if (!countdownRunning) {
        return;
    }

    countdownTimer->stop();
    countdownRunning = false;
}

void MainWindow::resetCountdown()
{
    countdownTimer->stop();
    countdownRunning = false;
    countdownRemainingMs = timerInputToMilliseconds();
    countdownTotalMs = countdownRemainingMs;
    countdownDisplay->display(formatCountdownTime(countdownRemainingMs));
    countdownProgress->setValue(0);

    if (blinkTimer->isActive()) {
        blinkTimer->stop();
        setWindowTitle(baseWindowTitle);
    }
}

void MainWindow::updateCountdown()
{
    if (!countdownRunning) {
        return;
    }

    countdownRemainingMs -= countdownTimer->interval();
    if (countdownRemainingMs < 0) {
        countdownRemainingMs = 0;
    }

    countdownDisplay->display(formatCountdownTime(countdownRemainingMs));

    if (countdownTotalMs > 0) {
        const int elapsed = countdownTotalMs - countdownRemainingMs;
        const int progress = (elapsed * 100) / countdownTotalMs;
        countdownProgress->setValue(progress);
    }

    if (countdownRemainingMs == 0) {
        finishCountdown();
    }
}

void MainWindow::finishCountdown()
{
    countdownTimer->stop();
    countdownRunning = false;
    countdownProgress->setValue(100);
    ++timerCompleteCount;
    refreshUsageStats();

    QApplication::alert(this, 0);
    QMessageBox::information(this, QStringLiteral("Таймер"), QStringLiteral("Время вышло!"));

    blinkTicks = 0;
    blinkFlag = false;
    blinkTimer->start();
}

void MainWindow::onPreset5Minutes()
{
    setTimerInputSeconds(5 * 60);
}

void MainWindow::onPreset10Minutes()
{
    setTimerInputSeconds(10 * 60);
}

void MainWindow::onPreset25Minutes()
{
    setTimerInputSeconds(25 * 60);
}

void MainWindow::setTimerInputSeconds(int seconds)
{
    if (seconds < 1) {
        seconds = 1;
    }

    const int hours = seconds / 3600;
    const int minutes = (seconds % 3600) / 60;
    const int secs = seconds % 60;
    timerInput->setTime(QTime(hours, minutes, secs));
}

int MainWindow::timerInputToMilliseconds() const
{
    const QTime time = timerInput->time();
    const int h = time.hour();
    const int m = time.minute();
    const int s = time.second();
    return ((h * 3600) + (m * 60) + s) * 1000;
}

void MainWindow::onTimerInputChanged()
{
    if (countdownRunning) {
        return;
    }

    countdownTotalMs = timerInputToMilliseconds();
    countdownRemainingMs = countdownTotalMs;
    countdownDisplay->display(formatCountdownTime(countdownRemainingMs));
    countdownProgress->setValue(0);
}

void MainWindow::blinkCompletionState()
{
    blinkFlag = !blinkFlag;
    setWindowTitle(blinkFlag ? QStringLiteral("Время вышло!") : baseWindowTitle);

    ++blinkTicks;
    if (blinkTicks >= 12) {
        blinkTimer->stop();
        setWindowTitle(baseWindowTitle);
    }
}

QString MainWindow::formatStopwatchTime(qint64 milliseconds) const
{
    const qint64 hours = milliseconds / 3600000;
    const qint64 minutes = (milliseconds / 60000) % 60;
    const qint64 seconds = (milliseconds / 1000) % 60;
    const qint64 msec = milliseconds % 1000;

    return QStringLiteral("%1:%2:%3.%4")
        .arg(hours, 2, 10, QLatin1Char('0'))
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'))
        .arg(msec, 3, 10, QLatin1Char('0'));
}

QString MainWindow::formatCountdownTime(int milliseconds) const
{
    if (milliseconds < 0) {
        milliseconds = 0;
    }

    const int totalSeconds = milliseconds / 1000;
    const int hours = totalSeconds / 3600;
    const int minutes = (totalSeconds % 3600) / 60;
    const int seconds = totalSeconds % 60;

    return QStringLiteral("%1:%2:%3")
        .arg(hours, 2, 10, QLatin1Char('0'))
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}

void MainWindow::refreshUsageStats()
{
    usageStatsLabel->setText(
        QStringLiteral("Статистика: запусков секундомера - %1 | завершённых таймеров - %2")
            .arg(stopwatchStartCount)
            .arg(timerCompleteCount));
}
