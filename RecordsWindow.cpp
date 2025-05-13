#include "RecordsWindow.h"
#include <QVBoxLayout>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QHeaderView>

RecordsWindow::RecordsWindow(QWidget* parent)
    : QDialog(parent), table(new QTableWidget(this)) {

    setWindowTitle("Таблица рекордов");
    resize(700, 500);

    setupTable();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(table);
    setLayout(layout);

    loadRecords();
}

void RecordsWindow::setupTable() {
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({ "Имя", "Завершено уровней", "Средняя скорость (сим/мин)" });
    table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->verticalHeader()->setVisible(false);
    table->setSortingEnabled(true);
    table->setColumnWidth(1, 200);
    table->setColumnWidth(2, 250);

}

void RecordsWindow::addRecordRow(const QString& name, int levelCount, float avgSpeed) {
    int row = table->rowCount();
    table->insertRow(row);

    table->setItem(row, 0, new QTableWidgetItem(name));
    table->setItem(row, 1, new QTableWidgetItem(QString::number(levelCount)));
    table->setItem(row, 2, new QTableWidgetItem(QString::number(avgSpeed, 'f', 1)));
}

void RecordsWindow::loadRecords() {
    records.clear();

    QDir dir("users");
    if (!dir.exists()) return;

    QFileInfoList fileList = dir.entryInfoList(QStringList() << "*.dat", QDir::Files);
    for (const QFileInfo& fileInfo : fileList) {
        QFile file(fileInfo.filePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;

        QTextStream in(&file);
        QString name;
        int levels = 0;
        float speed = 0.0;

        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.startsWith("Имя:"))
                name = line.section(":", 1).trimmed();
            else if (line.startsWith("Завершено уровней:"))
                levels = line.section(":", 1).trimmed().toInt();
            else if (line.startsWith("Средняя скорость:"))
                speed = line.section(":", 1).trimmed().toFloat();
        }

        if (!name.isEmpty())
            records.append({ name, levels, speed });
    }

    std::sort(records.begin(), records.end(), [](const Record& a, const Record& b) {
        if (a.levelsCompleted != b.levelsCompleted)
            return a.levelsCompleted > b.levelsCompleted;
        return a.averageSpeed > b.averageSpeed;
    });

    table->setRowCount(records.size());
    for (int i = 0; i < records.size(); ++i) {
        table->setItem(i, 0, new QTableWidgetItem(records[i].username));
        table->setItem(i, 1, new QTableWidgetItem(QString::number(records[i].levelsCompleted)));
        table->setItem(i, 2, new QTableWidgetItem(QString::number(records[i].averageSpeed, 'f', 1)));
    }
}
