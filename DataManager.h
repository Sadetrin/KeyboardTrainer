#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "User.h"

class DataManager {
public:
    static User loadUser(const QString& username);
    static void saveUser(const User& user);

private:
    static QString getUserFilePath(const QString& username);
};

#endif // DATAMANAGER_H
