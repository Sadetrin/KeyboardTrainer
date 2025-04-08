#pragma once

#include <QMainWindow>
#include <QTimer>
#include "Trainer.h"
#include "User.h"

QT_BEGIN_NAMESPACE
namespace Ui { class KeyboardTrainer; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartTraining();
    void onCharacterInput();
    void onTrainingFinished(int charactersTyped, int timeSpent, int errorCount);
    void updateStatus();
    void updateHighlightedReference();

private:
    void initUI();
    void updateStatusBar();

    Ui::KeyboardTrainer *ui;
    Trainer* trainer;
    User* user;
    QTimer* statusUpdateTimer;
    bool timerStarted;
};
