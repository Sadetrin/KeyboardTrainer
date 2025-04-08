#include "MainWindow.h"
#include "ui_KeyboardTrainer.h"
#include <QInputDialog>
#include <QPalette>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::KeyboardTrainer),
    trainer(nullptr),
    statusUpdateTimer(nullptr),
    timerStarted(false)
{
    ui->setupUi(this);

    QString username = QInputDialog::getText(this, "Введите имя пользователя", "Имя:");
    if (username.isEmpty()) username = "user";
    user = new User(username);

    initUI();
}

MainWindow::~MainWindow() {
    delete ui;
    delete user;
    delete trainer;
    delete statusUpdateTimer;
}

void MainWindow::initUI() {
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartTraining);
    connect(ui->inputField, &QLineEdit::textChanged, this, &MainWindow::onCharacterInput);

    statusUpdateTimer = new QTimer(this);
    connect(statusUpdateTimer, &QTimer::timeout, this, &MainWindow::updateStatus);

    updateStatusBar();
    updateStatus();
}

void MainWindow::updateStatus() {
    if (trainer) {
        QString text = trainer->getCurrentText();
        QString input = trainer->getUserInput();
        int percent = (text.isEmpty()) ? 0 : (input.length() * 100 / text.length());

        ui->progressLabel->setText(QString("Выполнено: %1%").arg(percent));
        ui->errorLabel->setText(QString("Ошибки: %1").arg(trainer->getErrorCount()));
        ui->timeLabel->setText(QString("Время: %1 сек").arg(trainer->getElapsedTime(), 0, 'f', 1));
    } else {
        ui->progressLabel->clear();
        ui->errorLabel->clear();
        ui->timeLabel->clear();
    }
}

void MainWindow::updateStatusBar() {
    ui->statusBar->showMessage(
        QString("Пользователь: %1 | Завершено уровней: %2 | Средняя скорость: %3 сим/мин")
            .arg(user->getUsername())
            .arg(user->getCompletedLevels())
            .arg(user->getAverageSpeed(), 0, 'f', 1)
        );
}

void MainWindow::onStartTraining() {
    if (trainer) delete trainer;
    trainer = new Trainer(user, this);

    Level::Difficulty difficulty = Level::Medium;
    QString levelText = ui->difficultyComboBox->currentText();
    if (levelText == "Easy") difficulty = Level::Easy;
    else if (levelText == "Medium") difficulty = Level::Medium;
    else if (levelText == "Hard") difficulty = Level::Hard;

    trainer->startTraining(difficulty);
    connect(trainer, &Trainer::trainingFinished, this, &MainWindow::onTrainingFinished);

    ui->referenceLabel->setText(trainer->getCurrentText());
    ui->referenceLabel->setTextFormat(Qt::RichText);
    ui->inputField->clear();
    ui->inputField->setEnabled(true);
    ui->inputField->setFocus();

    timerStarted = false;
    updateStatus();
    updateStatusBar();
    updateHighlightedReference();
}

void MainWindow::onCharacterInput() {
    if (!trainer) return;

    QString input = ui->inputField->text();
    if (!timerStarted && !input.isEmpty()) {
        statusUpdateTimer->start(100);
        trainer->startElapsedTimer();
        timerStarted = true;
    }

    trainer->updateUserInput(input);

    QString target = trainer->getCurrentText();
    QPalette palette = ui->inputField->palette();

    if (input.length() > 0 && input.length() <= target.length()) {
        palette.setColor(QPalette::Base, (input.back() != target[input.length() - 1]) ? QColor(255, 200, 200) : QColor(255, 255, 255));
    } else {
        palette.setColor(QPalette::Base, QColor(255, 255, 255));
    }
    ui->inputField->setPalette(palette);

    updateStatus();
    updateHighlightedReference();
}

void MainWindow::onTrainingFinished(int charactersTyped, int timeSpent, int errorCount) {
    ui->inputField->setEnabled(false);
    statusUpdateTimer->stop();

    float timeSpentF = trainer->getElapsedTime();
    int safeTime = qMax(1, static_cast<int>(timeSpentF));

    user->updateStatistics(charactersTyped, safeTime);
    updateStatusBar();

    ui->statusBar->showMessage(
        QString("Тренировка завершена! Символов: %1, Время: %2 сек, Ошибки: %3 | Завершено уровней: %4 | Средняя скорость: %5 сим/мин")
            .arg(charactersTyped)
            .arg(timeSpentF, 0, 'f', 1)
            .arg(errorCount)
            .arg(user->getCompletedLevels())
            .arg(user->getAverageSpeed(), 0, 'f', 1)
        );

    updateStatus();
}

void MainWindow::updateHighlightedReference() {
    if (!trainer) return;

    QString reference = trainer->getCurrentText();
    QString input = trainer->getUserInput();
    QString html;

    for (int i = 0; i < reference.length(); ++i) {
        if (i < input.length()) {
            html += QString("<span style='color: %1;'>%2</span>").arg(reference[i] == input[i] ? "green" : "red", reference[i]);
        } else if (i == input.length()) {
            html += QString("<u style='color: blue;'>%1</u>").arg(reference[i]);
        } else {
            html += QString("<span style='color: black;'>%1</span>").arg(reference[i]);
        }
    }

    ui->referenceLabel->setTextFormat(Qt::RichText);
    ui->referenceLabel->setText(html);
}
