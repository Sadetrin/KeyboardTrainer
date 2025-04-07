#ifndef APP_H
#define APP_H

#include<QWidget>
#include "User.h"
#include "DataManager.h"
#include "TypingSession.h"

class App : public QWidget {
    Q_OBJECT
public:
    explicit App(const QString& username, QWidget* parent = nullptr);

private:
    User currentUser;
    TypingSession* currentSession;

    void startNewSession(Level::Difficulty difficulty);
};

#endif // !APP_H
