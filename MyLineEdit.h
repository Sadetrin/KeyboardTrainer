#pragma once

#include <QLineEdit>
#include <QKeyEvent>

class MyLineEdit : public QLineEdit {
    Q_OBJECT

public:
    explicit MyLineEdit(QWidget* parent = nullptr);
    void resetFirstKeyDetection();


signals:
    void firstKeyPressed();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    bool firstKeyAlreadyPressed;
    bool firstKeyDetected = false;

};
