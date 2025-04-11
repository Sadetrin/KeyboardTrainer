#include "MainWindow.h"
#include "ui_KeyboardTrainer.h"
#include <QInputDialog>
#include <QPalette>
#include <QTimer>
#include <QDir>
#include <QFileInfoList>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::KeyboardTrainer),
    trainer(nullptr),
    statusUpdateTimer(nullptr),
    timerStarted(false)
{
    ui->setupUi(this);

    QString username;

    QStringList userFiles;
    QDir userDir("users");
    if (!userDir.exists()) {
        QDir().mkdir("users");
    }

    QFileInfoList fileList = userDir.entryInfoList(QStringList() << "*.dat", QDir::Files);
    for (const QFileInfo& fileInfo : fileList) {
        userFiles << fileInfo.baseName();
    }


    QStringList options;
    options << "Создать нового пользователя";
    options << userFiles;

    bool ok;
    QString selected = QInputDialog::getItem(this, "Выбор пользователя", "Выберите пользователя:", options, 0, false, &ok);

    if (ok && selected == "Создать нового пользователя") {
        username = QInputDialog::getText(this, "Новый пользователь", "Введите имя:");
        if (username.isEmpty()) username = "user";
    } else if (ok) {
        username = selected;
    } else {
        username = "user";
    }

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
    lastValidInput.clear();

}

void MainWindow::onCharacterInput() {
    if (!trainer) return;

    QString input = ui->inputField->text();
    QString target = trainer->getCurrentText();
    QPalette palette = ui->inputField->palette();

    bool hasError = false;

    // Проверка посимвольно, где начинается первая ошибка
    for (int i = 0; i < input.length(); ++i) {
        if (i >= target.length() || input[i] != target[i]) {
            hasError = true;
            break;
        }
    }

    if (hasError) {
        inputBlocked = true;
        palette.setColor(QPalette::Base, QColor(255, 200, 200)); // розовый фон
    } else {
        inputBlocked = false;
        lastValidInput = input;
        palette.setColor(QPalette::Base, QColor(255, 255, 255)); // белый фон
    }

    ui->inputField->setPalette(palette);

    // Блокируем ввод вручную, если ошибка и пользователь пытается продолжать
    if (inputBlocked && input.length() > lastValidInput.length()) {
        ui->inputField->blockSignals(true);
        ui->inputField->setText(input.left(lastValidInput.length() + 1)); // показываем ошибку, но не даем вводить дальше
        ui->inputField->blockSignals(false);
    }

    trainer->updateUserInput(input);

    if (!timerStarted && !input.isEmpty()) {
        statusUpdateTimer->start(100);
        trainer->startElapsedTimer();
        timerStarted = true;
    }

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
