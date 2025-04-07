#include "User.h"
#include <QFile>
#include <QTextStream>

User::User(const QString& username) : username(username), completedLevels(0), averageSpeed(0.0) {
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
        out << username << "\n" << completedLevels << "\n" << averageSpeed << "\n";
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
    return username + ".dat";
}
