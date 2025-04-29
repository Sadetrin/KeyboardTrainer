#include "Level.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRandomGenerator>

Level::Level(Difficulty difficulty) {
    text = getTextForDifficulty(difficulty);
}

QString Level::getTextForDifficulty(Difficulty difficulty) {
    QString fileName;

    switch (difficulty) {
    case VeryEasy:
        fileName = "resources/letters.txt";
        break;
    case Easy:
        fileName = "resources/words.txt";
        break;
    case Medium:
        fileName = "resources/sentences.txt";
        break;
    case Hard:
        fileName = "resources/long_sentences.txt";
        break;
    default:
        return "Уровень не определён.";
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть файл:" << fileName;
        return "Ошибка загрузки текста.";
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
        return "Файл пуст или содержит некорректные данные.";
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
        return "Уровень не определён.";
    }


    int index = QRandomGenerator::global()->bounded(lines.size());

    return lines[index];
}

QString Level::getText() const {
    return text;
}
