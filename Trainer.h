#ifndef TRAINER_H
#define TRAINER_H

#include <QObject>
#include <QElapsedTimer>
#include "User.h"
#include "Level.h"

class Trainer : public QObject {
    Q_OBJECT
public:
    Trainer(User* user, QObject* parent = nullptr);

    void startTraining(Level::Difficulty difficulty);
    void inputCharacter(QChar c);

    QString getCurrentText() const;
    QString getUserInput() const;
    int getErrorCount() const;
    int getElapsedTime() const;
    int getCurrentCharIndex() const;

signals:
    void trainingFinished(int charactersTyped, int timeSpent, int errorCount);

private:
    User* user;
    QString currentText;
    QString userInput;
    int errorCount;
    QElapsedTimer timer;
};

#endif // TRAINER_H
