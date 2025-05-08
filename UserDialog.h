#ifndef USERDIALOG_H
#define USERDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class UserDialog; }
QT_END_NAMESPACE

class UserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserDialog(const QStringList& users, QWidget *parent = nullptr);
    ~UserDialog();

    QString getSelectedUser() const;
    bool isNewUser() const;

protected:
    void reject() override; // Обработка закрытия диалога с подтверждением

private slots:
    void onCreateNewClicked();
    void onAccept();

private:
    Ui::UserDialog *ui;
    QString selectedUser;
    QStringList existingUsers;
    bool isNewUserFlag = false;
};

#endif // USERDIALOG_H
