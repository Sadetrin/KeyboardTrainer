#include "Trainer.h"

Trainer::Trainer(User* user, QObject* parent)
    : QObject(parent), user(user), errorCount(0) {}

void Trainer::startTraining(Level::Difficulty difficulty) {
    Level level(difficulty);
    currentText = level.getText();
    userInput.clear();
    errorCount = 0;
    timer.restart();
}

void Trainer::inputCharacter(QChar c) {
    if (userInput.size() < currentText.size()) {
        if (c != currentText.at(userInput.size())) {
            ++errorCount;
        }
        userInput += c;
        if (userInput.size() == currentText.size()) {
            int elapsed = getElapsedTime();
            user->updateStatistics(currentText.size(), elapsed);
            emit trainingFinished(currentText.size(), elapsed, errorCount);
        }
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

int Trainer::getCurrentCharIndex() const {
    return userInput.length();
}



int Trainer::getElapsedTime() const {
    return timer.elapsed() / 1000;
}
