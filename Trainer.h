#pragma once

#include "Level.h"
#include "User.h"
#include <QObject>
#include <QElapsedTimer>
#include <QSet>

class Trainer : public QObject {
    Q_OBJECT

public:
    explicit Trainer(User* user, QObject* parent = nullptr);

    void startTraining(Level::Difficulty difficulty);
    void inputCharacter(QChar c);
    void clearLastInput();
    void updateUserInput(const QString& newInput);
    void startElapsedTimer();

    QString getCurrentText() const;
    QString getUserInput() const;
    int getErrorCount() const;
    float getElapsedTime() const;
    int getCurrentCharIndex() const;
    QSet<int> getErrorIndices() const;  // возвращает индексы ошибок

signals:
    void trainingFinished(int charactersTyped, int timeSpent, int errorCount);

private:
    QString currentText;
    QString userInput;
    int errorCount;
    QSet<int> errorIndices;  // позиции, в которых были ошибки
    QElapsedTimer timer;
    User* user;
};
