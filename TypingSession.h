#ifndef TYPINGSESSION_H
#define TYPINGSESSION_H

#include "Level.h"
#include <QElapsedTimer>
#include <QString>


class TypingSession {
public:
    TypingSession(const Level& level);

    void start();
    bool processInput(const QString& input);

    double getSpeed() const;
    int getErrors() const;

private:
    Level level;
    QElapsedTimer timer;
    int errors;
    int totalChars;
};

#endif // TYPINGSESSION_H
