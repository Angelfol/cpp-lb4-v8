#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QElapsedTimer>
#include <QMainWindow>
#include <QString>

class QLabel;
class QLCDNumber;
class QListWidget;
class QProgressBar;
class QPushButton;
class QTabWidget;
class QTimeEdit;
class QTimer;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void startStopwatch();
    void stopStopwatch();
    void resetStopwatch();
    void saveLap();
    void updateStopwatchDisplay();

    void startCountdown();
    void pauseCountdown();
    void resetCountdown();
    void updateCountdown();
    void onTimerInputChanged();

    void onPreset5Minutes();
    void onPreset10Minutes();
    void onPreset25Minutes();
    void blinkCompletionState();

private:
    void setupUi();
    QWidget *createStopwatchTab();
    QWidget *createTimerTab();
    void finishCountdown();
    void setTimerInputSeconds(int seconds);
    int timerInputToMilliseconds() const;
    QString formatStopwatchTime(qint64 milliseconds) const;
    QString formatCountdownTime(int milliseconds) const;
    void refreshUsageStats();

    QString baseWindowTitle;

    QTabWidget *tabWidget = nullptr;
    QLabel *usageStatsLabel = nullptr;

    QLabel *stopwatchDisplay = nullptr;
    QPushButton *stopwatchStartButton = nullptr;
    QPushButton *stopwatchStopButton = nullptr;
    QPushButton *stopwatchResetButton = nullptr;
    QPushButton *stopwatchLapButton = nullptr;
    QListWidget *lapList = nullptr;
    QTimer *stopwatchTimer = nullptr;
    QElapsedTimer stopwatchElapsed;
    bool stopwatchRunning = false;
    qint64 stopwatchAccumulatedMs = 0;
    int lapCounter = 0;
    int stopwatchStartCount = 0;

    QTimeEdit *timerInput = nullptr;
    QLCDNumber *countdownDisplay = nullptr;
    QProgressBar *countdownProgress = nullptr;
    QPushButton *countdownStartButton = nullptr;
    QPushButton *countdownPauseButton = nullptr;
    QPushButton *countdownResetButton = nullptr;
    QTimer *countdownTimer = nullptr;
    QTimer *blinkTimer = nullptr;
    bool countdownRunning = false;
    int countdownTotalMs = 0;
    int countdownRemainingMs = 0;
    int timerCompleteCount = 0;
    int blinkTicks = 0;
    bool blinkFlag = false;
};

#endif // MAINWINDOW_H
