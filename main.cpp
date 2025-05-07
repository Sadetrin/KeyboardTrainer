#include "MainWindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QIcon>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setWindowIcon(QIcon(":/icons/icons/app_icon.png"));

    // Сначала проверяем выбор пользователя ДО создания главного окна
    QString username;
    if (!MainWindow::getUserName(username)) {
        return 0; // Пользователь отменил выбор
    }

    // Только после успешного выбора создаем главное окно
    MainWindow w(username);
    w.show();
    return a.exec();
}
