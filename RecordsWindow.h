#ifndef RECORDSWINDOW_H
#define RECORDSWINDOW_H

#include <QDialog>
#include <QTableWidget>

class RecordsWindow : public QDialog {
    Q_OBJECT

public:
    explicit RecordsWindow(QWidget* parent = nullptr);
    void loadRecords();

private:
    QTableWidget* table;

    void setupTable();
    void addRecordRow(const QString& name, int levelCount, float avgSpeed);

    struct Record {
        QString username;
        int levelsCompleted;
        float averageSpeed;
    };
    QList<Record> records;

};

#endif // RECORDSWINDOW_H
