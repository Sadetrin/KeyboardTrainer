#include "UserDialog.h"
#include "ui_UserDialog.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QIcon>

UserDialog::UserDialog(const QStringList& users, QWidget *parent)
    : QDialog(parent), ui(new Ui::UserDialog), selectedUser(""), existingUsers(users)
{
    ui->setupUi(this);
    setWindowTitle("Выбор пользователя");

    // Заполняем список пользователей
    ui->userComboBox->addItems(users);

    // Подключаем сигналы
    connect(ui->createUserButton, &QPushButton::clicked, this, &UserDialog::onCreateNewClicked);
    connect(ui->continueButton, &QPushButton::clicked, this, &UserDialog::onAccept);
    ui->createUserButton->setIcon(QIcon(":/icons/icons/user-add.svg"));
    ui->createUserButton->setIconSize(QSize(20,20));
    ui->createUserButton->setText("  Создать нового пользователя");

}

UserDialog::~UserDialog() {
    delete ui;
}

QString UserDialog::getSelectedUser() const {
    return selectedUser;
}

void UserDialog::onCreateNewClicked() {
    while (true) {
        bool ok;
        QString name = QInputDialog::getText(this, "Новый пользователь", "Введите имя:",
                                             QLineEdit::Normal, "", &ok);

        if (!ok) return; // Пользователь нажал "Отмена", вернуться к выбору

        name = name.trimmed().toLower();

        if (name.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Имя не может быть пустым.");
            continue;
        }

        if (existingUsers.contains(name)) {
            QMessageBox::warning(this, "Ошибка", "Пользователь с таким именем уже существует.");
            continue;
        }

        selectedUser = name;
        accept();
        return;
    }
}

void UserDialog::onAccept() {
    QString name = ui->userComboBox->currentText().trimmed().toLower();

    if (!name.isEmpty()) {
        selectedUser = name;
        accept();
    } else {
        QMessageBox::warning(this, "Ошибка", "Выберите пользователя или создайте нового.");
    }
}

void UserDialog::reject() {
    auto res = QMessageBox::question(this, "Подтверждение", "Вы точно хотите выйти?",
                                     QMessageBox::Yes | QMessageBox::No);
    if (res == QMessageBox::Yes)
        QDialog::reject();
    // Иначе остаёмся в диалоге
}
