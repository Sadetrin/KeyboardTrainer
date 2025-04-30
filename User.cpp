#include "User.h"
#include <QFile>
#include <QTextStream>
#include <QDir>

User::User(const QString& username) : username(username), completedLevels(0), averageSpeed(0.0) {
    QDir().mkpath(getUsersDirectory()); // Создание папки users, если не существует
    loadFromFile();
}

QString User::getUsername() const {
    return username;
}

int User::getCompletedLevels() const {
    return completedLevels;
}

double User::getAverageSpeed() const {
    return averageSpeed;
}

void User::updateStatistics(int charactersTyped, int timeSpent) {
    if (timeSpent > 0) {
        double newSpeed = (charactersTyped * 60.0) / timeSpent;
        averageSpeed = ((averageSpeed * completedLevels) + newSpeed) / (completedLevels + 1);
    }
    ++completedLevels;
    saveToFile();
}

void User::saveToFile() const {
    QFile file(getFilePath());
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "Имя: " << username << "\n";
        out << "Завершено уровней: " << completedLevels << "\n";
        out << "Средняя скорость: " << averageSpeed << "\n";
        file.close();
    }
}

bool User::loadFromFile() {
    QFile file(getFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    in >> username >> completedLevels >> averageSpeed;
    file.close();
    return true;
}

QString User::getFilePath() const {
    return getUsersDirectory() + "/" + username + ".dat";
}

QString User::getUsersDirectory() {
    return "users";
}
