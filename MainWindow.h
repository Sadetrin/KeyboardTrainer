#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "User.h"
#include "Trainer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class KeyboardTrainer; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onStartTraining();
    void onTrainingFinished(int charactersTyped, int timeSpent, int errorCount);
    void onCharacterInput();

private:
    Ui::KeyboardTrainer* ui;
    User* user;
    Trainer* trainer;

    void initUI();
    void updateStatus();
    void updateHighlightedReference();
};

#endif // MAINWINDOW_H
