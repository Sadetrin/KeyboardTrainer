#ifndef LEVEL_H
#define LEVEL_H

#include <QString>

class Level {
public:
    enum Difficulty {
        VeryEasy,
        Easy,
        Medium,
        Hard,
        VeryHard
    };

    static QString getTextForDifficulty(Difficulty difficulty);
    QString getText() const;

    Level(Difficulty difficulty);

private:
    QString text;
};

#endif // LEVEL_H
