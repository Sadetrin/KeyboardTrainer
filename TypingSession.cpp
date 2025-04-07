#include "TypingSession.h"

TypingSession::TypingSession(const Level& level)
    : level(level), errors(0), totalChars(0) {}

void TypingSession::start() {
    errors = 0;
    totalChars = 0;
    timer.start();
}

bool TypingSession::processInput(const QString& input) {
    const QString targetText = level.getText();

    if (input.length() > targetText.length()) return false;

    for (int i = 0; i < input.length(); ++i) {
        if (input[i] != targetText[i]) {
            ++errors;
        }
    }

    totalChars = input.length();

    return input == targetText;
}

double TypingSession::getSpeed() const {
    qint64 elapsedSeconds = timer.elapsed() / 1000;
    return (elapsedSeconds > 0) ? (totalChars * 60.0 / elapsedSeconds) : 0.0;
}

int TypingSession::getErrors() const {
    return errors;
}
