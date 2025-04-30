#ifndef RECORDS_H
#define RECORDS_H

#include <QString>
#include <QList>

struct Record {
    QString username;
    QString level;
    float timeSeconds;
    int charactersTyped;
    int errorCount;
    float speed;

    QString toString() const;
    static Record fromString(const QString& line);
};

void addRecord(const Record& record);
QList<Record> loadRecords();

#endif // RECORDS_H
