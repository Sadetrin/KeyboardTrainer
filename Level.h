#ifndef LEVEL_H
#define LEVEL_H

#include <QString>

class Level {
public:
    enum Difficulty {
        VeryEasy,   // Буквы
        Easy,       // Слова
        Medium,     // Предложение
        Hard        // Два или три предложения
    };

    explicit Level(Difficulty difficulty);
    QString getText() const;
    static QString getTextForDifficulty(Difficulty difficulty);

private:
    QString text;
};

#endif // LEVEL_H
