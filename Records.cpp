#include "records.h"
#include <QFile>
#include <QTextStream>
#include <QDir>

QString Record::toString() const {
    return QString("%1 | %2 | %3 | %4 | %5 | %6")
    .arg(username)
        .arg(level)
        .arg(timeSeconds, 0, 'f', 1)
        .arg(charactersTyped)
        .arg(errorCount)
        .arg(speed, 0, 'f', 1);
}

Record Record::fromString(const QString& line) {
    Record r;
    QStringList parts = line.split('|', Qt::SkipEmptyParts);
    if (parts.size() == 6) {
        r.username = parts[0].trimmed();
        r.level = parts[1].trimmed();
        r.timeSeconds = parts[2].trimmed().toFloat();
        r.charactersTyped = parts[3].trimmed().toInt();
        r.errorCount = parts[4].trimmed().toInt();
        r.speed = parts[5].trimmed().toFloat();
    }
    return r;
}

void addRecord(const Record& record) {
    QDir().mkpath("records");
    QFile file("records/records.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << record.toString() << "\n";
    }
}

QList<Record> loadRecords() {
    QList<Record> records;
    QFile file("records/records.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.trimmed().isEmpty()) {
                records.append(Record::fromString(line));
            }
        }
    }
    return records;
}
