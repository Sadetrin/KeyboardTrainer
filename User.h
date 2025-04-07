#ifndef USER_H
#define USER_H

#include <QString>

class User {
public:
    User(const QString& username);

    QString getUsername() const;
    int getCompletedLevels() const;
    double getAverageSpeed() const;

    void updateStatistics(int charactersTyped, int timeSpent);
    void saveToFile() const;
    bool loadFromFile();

private:
    QString username;
    int completedLevels;
    double averageSpeed;

    QString getFilePath() const;
};


#endif // !USER_H
