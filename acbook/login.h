#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <synchapi.h>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSplitter>
#include <QSpacerItem>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QStringList>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlRecord>
#include <QSettings>

/*注册界面的结构体*/
typedef struct regist_tag
{
    QWidget regist;
    QVBoxLayout layout;
    QLabel userlabel;
    QLabel passlabel;
    QLabel checkpass;
    QLineEdit useredit;
    QLineEdit passedit;
    QLineEdit checkedit;
    QSplitter userplitter;
    QSplitter passplitter;
    QSplitter checkpassspl;
    QPushButton con_reg;
} REG_DIA;

typedef struct create_bill_table
{
    QWidget create_bill;
    QVBoxLayout layout;
    QLabel edit_label;
    QLineEdit input_bill;
    QSplitter billplitter;
    QPushButton confirm_btn;
} CREATE_BILL_TABLE;
namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = 0);
    ~login();

private:
    Ui::login *ui;
    REG_DIA *reg;
    CREATE_BILL_TABLE *create_bill;
    QSqlDatabase user_db;
    QString username;
    QString dbfilename;
    QString password;

    bool remeberasswd;
    bool autologin;
    QSqlDatabase open_database();
    bool check_user_pass_table(QSqlDatabase db);
    void get_bill_name(QSqlDatabase db);
    void save_user_pass();
    void load_user_pass();
    void create_bill_ui_init(CREATE_BILL_TABLE *bill);
    void add_bill_table(QString billname);
private slots:
    int loginbtn_slot();
    int register_slot();
    int regist_confirm_btn_clicked_slot();
    int add_bill_to_table();
public:

    QString user;
    QStringList bills;
    int set_login_status(bool status);
    QString *get_current_login_user();
    int clear_log_ui();


};

bool get_login_status();
QSqlDatabase get_acbook_db();
QString *get_current_user();


#endif // LOGIN_H
