#include "MainWindow.h"
#include "ui_KeyboardTrainer.h"
#include <QInputDialog>
#include <QPalette>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::KeyboardTrainer),
    trainer(nullptr)
{
    ui->setupUi(this);

    // Ввод имени пользователя
    QString username = QInputDialog::getText(this, "Введите имя пользователя", "Имя:");
    if (username.isEmpty()) username = "user";

    user = new User(username);

    initUI();
}

MainWindow::~MainWindow() {
    delete ui;
    delete user;
    delete trainer;
}

void MainWindow::initUI() {
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartTraining);
    connect(ui->inputField, &QLineEdit::textChanged, this, &MainWindow::onCharacterInput);

    updateStatus();

    ui->statusBar->showMessage(
        QString("Пользователь: %1 | Завершено уровней: %2 | Средняя скорость: %3 сим/мин")
            .arg(user->getUsername())
            .arg(user->getCompletedLevels())
            .arg(user->getAverageSpeed(), 0, 'f', 1)
        );
}

void MainWindow::updateStatus() {
    if (trainer) {
        QString text = trainer->getCurrentText();
        QString input = trainer->getUserInput();
        int percent = (text.isEmpty()) ? 0 : (input.length() * 100 / text.length());

        ui->progressLabel->setText(QString("Выполнено: %1%").arg(percent));
        ui->errorLabel->setText(QString("Ошибки: %1").arg(trainer->getErrorCount()));
        ui->timeLabel->setText(QString("Время: %1 сек").arg(trainer->getElapsedTime()));
    } else {
        ui->progressLabel->clear();
        ui->errorLabel->clear();
        ui->timeLabel->clear();
    }
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

    ui->inputField->clear();
    ui->inputField->setEnabled(true);
    updateHighlightedReference();
    updateStatus();
}

void MainWindow::onCharacterInput() {
    if (!trainer) return;

    QString input = ui->inputField->text();
    QString target = trainer->getCurrentText();

    int lastIndex = input.length() - 1;

    if (lastIndex >= 0 && lastIndex < target.length()) {
        QChar inputChar = input.at(lastIndex);
        trainer->inputCharacter(inputChar);

        QPalette palette = ui->inputField->palette();
        if (inputChar != target.at(lastIndex)) {
            palette.setColor(QPalette::Base, QColor(255, 200, 200)); // красный при ошибке
        } else {
            palette.setColor(QPalette::Base, QColor(255, 255, 255)); // белый при правильном вводе
        }
        ui->inputField->setPalette(palette);

        updateStatus();
        updateHighlightedReference();
    }
}

void MainWindow::onTrainingFinished(int charactersTyped, int timeSpent, int errorCount) {
    ui->inputField->setEnabled(false);

    // Обновление статистики пользователя
    user->updateStatistics(charactersTyped, timeSpent);

    // Обновлённый статус в строке состояния
    ui->statusBar->showMessage(
        QString("Тренировка завершена! Символов: %1, Время: %2 сек, Ошибки: %3 | Завершено уровней: %4, Средняя скорость: %5 сим/мин")
            .arg(charactersTyped)
            .arg(timeSpent)
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
    int currentIndex = trainer->getCurrentCharIndex();

    QString html;

    for (int i = 0; i < reference.length(); ++i) {
        if (i < input.length()) {
            if (reference[i] == input[i]) {
                html += QString("<span style='color: green;'>%1</span>").arg(reference[i]);
            } else {
                html += QString("<span style='color: red;'>%1</span>").arg(reference[i]);
            }
        } else if (i == currentIndex) {
            html += QString("<u><span style='color: blue;'>%1</span></u>").arg(reference[i]);
        } else {
            html += QString("<span style='color: black;'>%1</span>").arg(reference[i]);
        }
    }

    ui->referenceLabel->setTextFormat(Qt::RichText);
    ui->referenceLabel->setText(html);
}
