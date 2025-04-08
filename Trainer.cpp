#include "Trainer.h"

Trainer::Trainer(User* user, QObject* parent)
    : QObject(parent), errorCount(0), user(user) {}

void Trainer::startTraining(Level::Difficulty difficulty) {
    Level level(difficulty);
    currentText = level.getText();
    userInput.clear();
    errorCount = 0;
    errorIndices.clear();
    timer.invalidate();
}

void Trainer::inputCharacter(QChar c) {
    if (userInput.length() < currentText.length()) {
        userInput.append(c);
        int index = userInput.length() - 1;

        if (c != currentText[index]) {
            if (!errorIndices.contains(index)) {
                errorIndices.insert(index);
                ++errorCount;
            }
        }

        if (userInput.length() == currentText.length()) {
            int elapsedMs = timer.elapsed();
            if (elapsedMs <= 0) elapsedMs = 1;
            emit trainingFinished(currentText.length(), elapsedMs, errorCount);
        }
    }
}

void Trainer::clearLastInput() {
    if (!userInput.isEmpty()) {
        int index = userInput.length() - 1;
        userInput.chop(1);
        // Не удаляем ошибку из errorIndices и не уменьшаем errorCount
    }
}

QString Trainer::getCurrentText() const {
    return currentText;
}

QString Trainer::getUserInput() const {
    return userInput;
}

int Trainer::getErrorCount() const {
    return errorCount;
}

float Trainer::getElapsedTime() const {
    return timer.isValid() ? timer.elapsed() / 1000.0f : 0.0f;
}

int Trainer::getCurrentCharIndex() const {
    return userInput.length();
}

void Trainer::updateUserInput(const QString& newInput) {
    userInput = newInput;

    // errorCount и errorIndices не очищаются — сохраняем прежние ошибки
    for (int i = 0; i < std::min(userInput.length(), currentText.length()); ++i) {
        if (userInput[i] != currentText[i]) {
            if (!errorIndices.contains(i)) {
                errorIndices.insert(i);
                ++errorCount;
            }
        }
    }

    if (userInput == currentText) {
        int elapsedTime = timer.elapsed();
        if (elapsedTime <= 0) elapsedTime = 1;
        emit trainingFinished(currentText.length(), elapsedTime, errorCount);
    }
}

void Trainer::startElapsedTimer() {
    timer.restart();
}

QSet<int> Trainer::getErrorIndices() const {
    return errorIndices;
}
