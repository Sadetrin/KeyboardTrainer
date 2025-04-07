#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_KeyboardTrainer.h"

class KeyboardTrainer : public QMainWindow
{
    Q_OBJECT

public:
    KeyboardTrainer(QWidget *parent = nullptr);
    ~KeyboardTrainer();

private:
    Ui::KeyboardTrainer ui;
};
