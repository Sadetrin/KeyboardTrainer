#include "MyLineEdit.h"

MyLineEdit::MyLineEdit(QWidget* parent)
    : QLineEdit(parent), firstKeyAlreadyPressed(false) {}

void MyLineEdit::keyPressEvent(QKeyEvent* event) {
    if (!firstKeyAlreadyPressed && !event->text().isEmpty() && !event->text().at(0).isSpace()) {
        emit firstKeyPressed();
        firstKeyAlreadyPressed = true;
    }

    QLineEdit::keyPressEvent(event);
}

void MyLineEdit::resetFirstKeyDetection() {
    firstKeyDetected = false;
}
