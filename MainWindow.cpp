#include "MainWindow.h"
#include "ui_KeyboardTrainer.h"
#include "Records.h"
#include <QInputDialog>
#include <QPalette>
#include <QTimer>
#include <QDir>
#include <QFileInfoList>
#include <QMessageBox>
#include "recordswindow.h"


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

    bool userSelected = false;
    while (!userSelected) {
        bool ok;
        QString selected = QInputDialog::getItem(this, "Выбор пользователя", "Выберите пользователя:", options, 0, false, &ok);

        if (!ok) {
            QApplication::quit();
            return; // Прерываем конструктор MainWindow
        }

        if (selected == "Создать нового пользователя") {
            QString newName;
            bool nameOk;
            while (true) {
                newName = QInputDialog::getText(this, "Новый пользователь", "Введите имя:", QLineEdit::Normal, "", &nameOk);

                if (!nameOk) break; // Назад к выбору пользователя

                if (newName.trimmed().isEmpty()) {
                    QMessageBox::warning(this, "Ошибка", "Имя не может быть пустым.");
                    continue;
                }

                if (userFiles.contains(newName)) {
                    QMessageBox::warning(this, "Ошибка", "Пользователь с таким именем уже существует.");
                    continue;
                }

                username = newName;
                userSelected = true;
                break;
            }
        } else {
            username = selected;
            userSelected = true;
        }
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

    // Заполнение уровней сложности
    ui->difficultyComboBox->clear();
    ui->difficultyComboBox->addItem("Очень легко"); // VeryEasy
    ui->difficultyComboBox->addItem("Легко");       // Easy
    ui->difficultyComboBox->addItem("Средне");      // Medium
    ui->difficultyComboBox->addItem("Сложно");      // Hard

    statusUpdateTimer = new QTimer(this);
    connect(statusUpdateTimer, &QTimer::timeout, this, &MainWindow::updateStatus);

    updateStatusBar();
    updateStatus();
    connect(ui->recordsButton, &QPushButton::clicked, this, [this]() {
        if (!recordsWindow) {
            recordsWindow = new RecordsWindow(this);
        }
        recordsWindow->show();
        recordsWindow->raise();
        recordsWindow->activateWindow();
    });
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

    if (levelText == "Очень легко") difficulty = Level::VeryEasy;
    else if (levelText == "Легко") difficulty = Level::Easy;
    else if (levelText == "Средне") difficulty = Level::Medium;
    else if (levelText == "Сложно") difficulty = Level::Hard;

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
    Record record;
    record.username = user->getUsername();
    record.level = ui->difficultyComboBox->currentText();
    record.timeSeconds = timeSpentF;
    record.charactersTyped = charactersTyped;
    record.errorCount = errorCount;
    record.speed = user->getAverageSpeed(); // или рассчитать прямо здесь

    addRecord(record);

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


