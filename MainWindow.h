#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Trainer.h"
#include "User.h"
#include "RecordsWindow.h"
#include <QMediaPlayer>
#include <QAudioOutput>


QT_BEGIN_NAMESPACE
namespace Ui { class KeyboardTrainer; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    static bool getUserName(QString& outUsername); // Статический метод для выбора пользователя
    explicit MainWindow(const QString& username, QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onStartTraining();
    void onCharacterInput();
    void onTrainingFinished(int charactersTyped, int timeSpent, int errorCount);
    void onMuteButtonClicked();
    void onQuitButtonClicked();

private:
    Ui::KeyboardTrainer* ui;
    Trainer* trainer;
    User* user;
    QTimer* statusUpdateTimer;
    bool timerStarted;

    void initUI();
    void updateStatus();
    void updateStatusBar();
    void updateHighlightedReference();
    QString lastValidInput;  // Последняя корректная строка
    bool inputBlocked = false;
    RecordsWindow* recordsWindow = nullptr;
    QMediaPlayer *musicPlayer;
    QAudioOutput *audioOutput;
    bool isMuted = false;
    float currentVolume = 0.1f;
    QIcon soundOnIcon;
    QIcon soundOffIcon;

};

#endif // MAINWINDOW_H
