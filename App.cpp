#include "App.h"

App::App(const QString& username, QWidget* parent)
    : QWidget(parent), currentUser(username), currentSession(nullptr) {
    // Логика инициализации интерфейса будет добавлена позже
}

void App::startNewSession(Level::Difficulty difficulty) {
    if (currentSession) delete currentSession;
    currentSession = new TypingSession(Level(difficulty));
    currentSession->start();
}
