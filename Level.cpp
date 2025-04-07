#include "Level.h"

Level::Level(Difficulty difficulty) {
    text = getTextForDifficulty(difficulty);
}

QString Level::getTextForDifficulty(Difficulty difficulty) {
    switch (difficulty) {
    case Easy:
        return "Простой текст для тренировки.";
    case Medium:
        return "Это текст средней сложности, предназначенный для практики.";
    case Hard:
        return "Сложный уровень требует более высокой концентрации и скорости.";
    default:
        return "Уровень не определён.";
    }
}

QString Level::getText() const {
    return text;
}
