#include "Level.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRandomGenerator>
#include <stdexcept>

Level::Level(Difficulty difficulty) {
    text = getTextForDifficulty(difficulty);
}

QString Level::getTextForDifficulty(Difficulty difficulty) {
    QString fileName;

    switch (difficulty) {
    case VeryEasy:
        fileName = ":/resources/resources/letters.txt";
        break;
    case Easy:
        fileName = ":/resources/resources/words.txt";
        break;
    case Medium:
        fileName = ":/resources/resources/sentences.txt";
        break;
    case Hard:
        fileName = ":/resources/resources/long_sentences.txt";
        break;
    default:
        throw std::runtime_error("Неизвестный уровень сложности");
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString errorMsg = QString("Не удалось открыть файл уровня сложности: %1").arg(fileName);
        qCritical() << errorMsg;
        throw std::runtime_error(errorMsg.toStdString());
    }

    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty())
            lines << line;
    }

    file.close();

    if (lines.isEmpty()) {
        QString errorMsg = QString("Файл уровня сложности пуст: %1").arg(fileName);
        qCritical() << errorMsg;
        throw std::runtime_error(errorMsg.toStdString());
    }

    switch (difficulty) {
    case VeryEasy: {
        QString allLetters = lines.join("").remove(' ');
        std::shuffle(allLetters.begin(), allLetters.end(), *QRandomGenerator::global());

        QString spaced;
        for (QChar ch : allLetters.left(30)) {
            spaced += ch;
            spaced += ' ';
        }
        return spaced.trimmed();
    }

    case Easy: {
        std::shuffle(lines.begin(), lines.end(), *QRandomGenerator::global());
        return lines.mid(0, 10).join(" ");
    }

    case Medium: {
        std::shuffle(lines.begin(), lines.end(), *QRandomGenerator::global());
        return lines.mid(0, 2).join(" ");
    }

    case Hard: {
        std::shuffle(lines.begin(), lines.end(), *QRandomGenerator::global());
        return lines.mid(0, 2).join(" ");
    }

    default:
        throw std::runtime_error("Неизвестный уровень сложности");
    }


    int index = QRandomGenerator::global()->bounded(lines.size());

    return lines[index];
}

QString Level::getText() const {
    return text;
}
