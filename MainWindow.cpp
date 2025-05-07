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
#include "UserDialog.h"


// bool MainWindow::getUserName(QString& outUsername)
// {
//     QStringList userFiles;
//     QDir userDir("users");
//     if (!userDir.exists()) {
//         QDir().mkdir("users");
//     }

//     QFileInfoList fileList = userDir.entryInfoList(QStringList() << "*.dat", QDir::Files);
//     for (const QFileInfo& fileInfo : fileList) {
//         userFiles << fileInfo.baseName();
//     }

//     QStringList options;
//     options << "Создать нового пользователя";
//     options << userFiles;

//     while (true) {
//         bool ok;
//         QString selected = QInputDialog::getItem(nullptr, "Выбор пользователя",
//                                                  "Выберите пользователя:", options, 0, false, &ok);

//         if (!ok) {
//             QMessageBox::StandardButton confirm = QMessageBox::question(
//                 nullptr,
//                 "Подтверждение",
//                 "Вы точно хотите выйти?",
//                 QMessageBox::Yes | QMessageBox::No
//                 );
//             return (confirm == QMessageBox::Yes) ? false : getUserName(outUsername); // Рекурсия с подтверждением
//         }

//         if (selected == "Создать нового пользователя") {
//             while (true) {
//                 bool nameOk;
//                 QString newName = QInputDialog::getText(nullptr, "Новый пользователь",
//                                                         "Введите имя:", QLineEdit::Normal, "", &nameOk);

//                 if (!nameOk) break; // Вернуться к выбору пользователя

//                 newName = newName.trimmed().toLower();

//                 if (newName.isEmpty()) {
//                     QMessageBox::warning(nullptr, "Ошибка", "Имя не может быть пустым.");
//                     continue;
//                 }

//                 if (userFiles.contains(newName)) {
//                     QMessageBox::warning(nullptr, "Ошибка", "Пользователь с таким именем уже существует.");
//                     continue;
//                 }

//                 outUsername = newName;
//                 return true;
//             }
//         } else {
//             outUsername = selected.trimmed().toLower();
//             return true;
//         }
//     }
// }



bool MainWindow::getUserName(QString& outUsername)
{
    QStringList userFiles;
    QDir userDir("users");
    if (!userDir.exists()) {
        QDir().mkdir("users");
    }

    QFileInfoList fileList = userDir.entryInfoList(QStringList() << "*.dat", QDir::Files);
    for (const QFileInfo& fileInfo : fileList) {
        userFiles << fileInfo.baseName();
    }

    while (true) {
        UserDialog dialog(userFiles); // создаём диалог
        int result = dialog.exec();

        if (result == QDialog::Rejected) {
            QMessageBox::StandardButton confirm = QMessageBox::question(
                nullptr,
                "Подтверждение",
                "Вы точно хотите выйти?",
                QMessageBox::Yes | QMessageBox::No
                );
            if (confirm == QMessageBox::Yes)
                return false;
            else
                continue;
        }

        QString selected = dialog.getSelectedUser();
        if (selected.isEmpty()) {
            QMessageBox::warning(nullptr, "Ошибка", "Имя не может быть пустым.");
            continue;
        }

        outUsername = selected.trimmed().toLower();
        return true;
    }
}


MainWindow::MainWindow(const QString& username, QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::KeyboardTrainer),
    trainer(nullptr),
    user(new User(username)),
    statusUpdateTimer(nullptr),
    timerStarted(false),
    recordsWindow(nullptr),
    musicPlayer(nullptr),
    audioOutput(nullptr)
{
    ui->setupUi(this);

    //setFixedSize(size());
    QApplication::setFont(QFont("Comic Sans MS", 12));

    initUI();

    ui->progressValue->setText("<html><img src=':/icons/icons/diagram.svg' width='20' style='vertical-align:middle;'> Выполнено: 0%</html>");
    ui->progressValue->setTextFormat(Qt::RichText);
    ui->errorsValue->setText("<html><img src=':/icons/icons/cross.svg' width='20' style='vertical-align:middle;'> Ошибки: 0</html>");
    ui->errorsValue->setTextFormat(Qt::RichText);
    ui->timeLabel->setText("<html><img src=':/icons/icons/clock.svg' width='20' style='vertical-align:middle;'> Время: 0.0 сек</html>");
    ui->timeLabel->setTextFormat(Qt::RichText);
    ui->referenceLabel->setText("<html><div style='color:gray;font-style:italic;'>Выберите сложность и нажмите 'Старт'</div></html>");
    ui->referenceLabel->setTextFormat(Qt::RichText);

    updateStatusBar();


    musicPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    musicPlayer->setAudioOutput(audioOutput);
    audioOutput->setVolume(currentVolume); // Громкость от 0.0 до 1.0

    musicPlayer->setSource(QUrl("qrc:/music/music/background.mp3"));
    if (musicPlayer->mediaStatus() == QMediaPlayer::NoMedia) {
        qDebug() << "Ошибка: музыкальный файл не найден!";
    } else {
        musicPlayer->play();
    }

    connect(musicPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            musicPlayer->setPosition(0);
            musicPlayer->play();
        }
    });

    connect(musicPlayer, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error error) {
        QString errorMsg;
        switch (error) {
        case QMediaPlayer::ResourceError: errorMsg = "Файл не найден"; break;
        case QMediaPlayer::FormatError: errorMsg = "Неподдерживаемый формат"; break;
        case QMediaPlayer::NetworkError: errorMsg = "Ошибка сети"; break;
        default: errorMsg = "Неизвестная ошибка";
        }
        QMessageBox::critical(this, "Ошибка музыки", errorMsg);
    });


    soundOnIcon=QIcon(":/icons/icons/volume-on.svg");
    soundOffIcon=QIcon(":/icons/icons/volume-off.svg");

    ui->muteButton->setIcon(soundOnIcon);  // Стартовая иконка
    ui->muteButton->setIconSize(QSize(24, 24)); // Размер иконки
    ui->muteButton->setStyleSheet(
        "QPushButton { border: none; background: transparent; }"
        "QPushButton:pressed { padding: 1px; }"
        );

    connect(ui->muteButton, &QPushButton::clicked, this, &MainWindow::onMuteButtonClicked);

}


MainWindow::~MainWindow() {
    delete ui;
    delete user;
    delete trainer;
    delete statusUpdateTimer;
    delete musicPlayer;
    delete audioOutput;
    delete recordsWindow;
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

        QString progressText = QString("Выполнено: %1%").arg(percent);
        ui->progressValue->setText(
            QString("<html><img src=':/icons/icons/diagram.svg' width='20' style='vertical-align:middle;'> %1</html>")
                .arg(progressText)
            );
        ui->progressValue->setTextFormat(Qt::RichText);


        QString errorsText = QString("Ошибки: %1").arg(trainer->getErrorCount());
        ui->errorsValue->setText(QString("<html><img src=':/icons/icons/cross.svg' width='20' style='vertical-align:middle;'> %1</html>").arg(errorsText));
        ui->errorsValue->setTextFormat(Qt::RichText);

        QString timeText = QString("Время: %1 сек").arg(trainer->getElapsedTime(), 0, 'f', 1);
        ui->timeLabel->setText(QString("<html><img src=':/icons/icons/clock.svg' width='22' style='vertical-align:middle;'> %1</html>").arg(timeText));
        ui->timeLabel->setTextFormat(Qt::RichText);
    } else {
        ui->progressValue->clear();
        ui->errorsValue->clear();
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
    record.speed = user->getAverageSpeed();

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


void MainWindow::onMuteButtonClicked() {
    isMuted = !isMuted;

    if (isMuted) {
        currentVolume=audioOutput->volume();
        audioOutput->setVolume(0);
        ui->muteButton->setIcon(soundOffIcon);
    } else {
        audioOutput->setVolume(currentVolume);
        ui->muteButton->setIcon(soundOnIcon);
    }
}

