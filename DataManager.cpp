#include "DataManager.h"
#include <QDir>

QString DataManager::getUserFilePath(const QString& username) {
    return QDir::currentPath() + "/" + username + ".dat";
}

User DataManager::loadUser(const QString& username) {
    User user(username);
    user.loadFromFile();
    return user;
}

void DataManager::saveUser(const User& user) {
    user.saveToFile();
}
