#include "login.h"
#include "ui_login.h"
#include <QDebug>
#include <iostream>
#include <QWidget>
#include <QVBoxLayout>
#include <QSplitter>
#include <QSpacerItem>
#include <QPushButton>
#include <QStringListIterator>
#include <QStringList>
#include <QMessageBox>
#include <Qtsql/QSqlQuery>
#include <Qtsql/QSqlRecord>
#include <QDate>
#include <QFont>
#include <QLine>
#include <stdio.h>
bool login_flag = false;
QSqlDatabase g_db;
QString *g_user;
login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);

    /*设置窗口标题*/
    this->setWindowTitle(tr("登录"));
    ui->username->setEchoMode(QLineEdit::Normal);

    ui->password->setEchoMode(QLineEdit::Password);

    /*注册窗口*/
    this->reg = new REG_DIA;
    this->create_bill = new CREATE_BILL_TABLE;

    create_bill_ui_init(this->create_bill);

    /*信号与槽*/
    connect(ui->logbtn,SIGNAL(clicked()),this,SLOT(loginbtn_slot()));
    connect(ui->registerbtn,SIGNAL(clicked()),this,SLOT(register_slot()));
    connect(&this->create_bill->confirm_btn,SIGNAL(clicked()),this,SLOT(add_bill_to_table()));


}


/*析构函数*/
login::~login()
{
    delete ui;
}


/*登录按键的槽函数*/
int login::loginbtn_slot()
{
    if(ui->username->text().isEmpty())
    {
        QMessageBox::critical(this,"错误","请输入用户名");
        return 0;
    }
    if(ui->password->text().isEmpty())
    {
        QMessageBox::critical(this,"错误","请输入密码");
        return 0;
    }

    this->username = ui->username->text();

    QSqlDatabase db =  open_database();
    /*查询该用户是否注册*/

    QStringList tables = db.tables();
    QStringListIterator *itr = new QStringListIterator(tables);
    bool isFind = false;
    while (itr->hasNext())
    {
        QString tableNmae = itr->next().toLocal8Bit();
        QString real_name = tableNmae.split("_").at(0);
        if(real_name.compare(this->ui->username->text()) == 0)
        {
            isFind = true;
            break;
        }
    }
    if(isFind != true)
    {
        QMessageBox::information(this,"信息","该用户未注册");
        return 0;
    }

    /*查询用户名及密码匹配*/
    QSqlQuery sql_query;
    QString select_sql = "select username, password from userpass";
    QString get_pass;
    if(!sql_query.exec(select_sql))
    {
        qDebug()<< sql_query.lastError();
    }
    else
    {
        isFind = false;
        while(sql_query.next())
        {
            QString username = sql_query.value("username").toString();
            if(username.compare(this->ui->username->text()) == 0)
            {

                get_pass = sql_query.value(1).toString();
                isFind = true;
                break;
            }
        }
        if(isFind == false)
        {
            QMessageBox::information(this,"信息","该用户未注册");
        }
        else
        {

            if(get_pass.compare(this->ui->password->text()) != 0)
            {
                QMessageBox::information(this,"信息","密码不正确");
                return 0;
            }
            else
            {

                qDebug( "login successful");
                this->user = this->ui->username->text();
                g_user = new QString(this->user);
                qDebug() << *g_user;
            }
        }
    }

    this->hide();
    login_flag = true;
    get_bill_name(db);
    return 0;
}


/*注册按键的槽函数*/
int login::register_slot()
{
    /*注册窗口*/
    this->hide();

    /*设置注册窗口的标签名*/
    this->reg->regist.setStyleSheet("background-color: #ffffff");
    this->reg->useredit.setStyleSheet("background:transparent;border-width:1;border-left-style:outset");
    this->reg->useredit.setFont(QFont("Times New Roman",12));

    this->reg->passedit.setStyleSheet("background:transparent;border-width:1;border-left-style:outset");
    this->reg->passedit.setFont(QFont("Times New Roman",12));

    this->reg->checkedit.setStyleSheet("background:transparent;border-width:1;border-left-style:outset");
    this->reg->checkedit.setFont(QFont("Times New Roman",12));

    this->reg->con_reg.setStyleSheet("QPushButton { \
                                     width: 85px; height: 30px; color: #fff; border: 0px solid rgba(255, 255, 255, 0); \
                                     border-radius: 5px; background-color: #3bafda; } \
                                 QPushButton:hover { \
                                     background-color: #4fc1e9; \
                                 } \
                                 QPushButton:pressed { \
                                     background: qradialgradient(cx:0.5, \
                                     cy: 0.5, fx: 0.5, fy: 0.5, radius: 1.5, \
                                     stop: 0.2 #4fc1e9, stop: 0.8 #3bafda);}");
    this->reg->userlabel.setText(tr("输入用户:"));
    this->reg->passlabel.setText(tr("输入密码:"));
    this->reg->checkpass.setText(tr("确认密码:"));
    this->reg->con_reg.setText(tr("注册"));

    /*添加用户名编辑窗口*/
    this->reg->userplitter.addWidget(&this->reg->userlabel);
    this->reg->userplitter.addWidget(&this->reg->useredit);

    /*添加密码编辑窗口*/
    this->reg->passplitter.addWidget(&this->reg->passlabel);
    this->reg->passplitter.addWidget(&this->reg->passedit);
    this->reg->passedit.setEchoMode(QLineEdit::Password);

    /*添加密码确认编辑窗口*/
    this->reg->checkpassspl.addWidget(&this->reg->checkpass);
    this->reg->checkpassspl.addWidget(&this->reg->checkedit);
    this->reg->checkedit.setEchoMode(QLineEdit::Password);
    this->reg->regist.setFixedSize(320,240);
    this->reg->con_reg.setFixedSize(80,30);

    QVBoxLayout *layout = new QVBoxLayout(&this->reg->regist);
    layout->addWidget(&this->reg->userplitter);

    layout->addSpacerItem(new QSpacerItem(20,20));
    layout->addWidget(&this->reg->passplitter);

    layout->addSpacerItem(new QSpacerItem(20,20));
    layout->addWidget(&this->reg->checkpassspl);
    layout->addSpacerItem(new QSpacerItem(10,60));

    /*注册按钮*/
    layout->addWidget(&this->reg->con_reg,0,Qt::AlignCenter);
    this->reg->regist.setWindowTitle(tr("注册"));
    this->reg->regist.show();
    connect(&this->reg->con_reg,SIGNAL(clicked()),this,SLOT(regist_confirm_btn_clicked_slot()));
    return 0;

}


/*获取当前登录状态*/
bool get_login_status()
{
    qDebug("judge %d\n",login_flag);
    return login_flag;
}

/*设置当前登录状态*/
int login::set_login_status(bool status)
{
    login_flag = status;
    return 0;
}

/*注册窗口中的注册按钮槽函数*/
int login::regist_confirm_btn_clicked_slot()
{
    qDebug("register btn\n");

    if(this->reg->useredit.text().isEmpty() || this->reg->passedit.text().isEmpty() \
            || this->reg->checkedit.text().isEmpty())
    {
         QMessageBox::information(this,"通知","未填写用户名或密码!\n");
         return -1;
    }

    QSqlDatabase db = open_database();
    QStringList tables = db.tables();

    /*检测用户是否已注册*/
    QStringListIterator *itr = new QStringListIterator(tables);
    bool isFind = false;
    while (itr->hasNext())
    {
        QString tableNmae = itr->next().toLocal8Bit();
        QString userame = tableNmae.split("_").at(0);
        qDebug() << userame;
        if(userame.compare( this->reg->useredit.text()) == 0)
        {
            qDebug("is exist!\n");
            isFind = true;
        }
    }
    if(isFind == true)
    {
        QMessageBox::information(this,"警告","用户已存在!\n");
        return 0;
    }

    /*检测两次密码输入是否匹配*/
    if(this->reg->passedit.text().compare(this->reg->checkedit.text()) != 0)
    {
        QMessageBox::information(this,"通知","两次密码不匹配!\n");
        this->reg->passedit.clear();
        return 0;
    }

    /*写入到userpass表中*/
    QSqlQuery sql_query;
    QString insert_sql = "insert into userpass values (?,?)";
    sql_query.prepare(insert_sql);
    sql_query.addBindValue(this->reg->useredit.text());
    sql_query.addBindValue(this->reg->passedit.text());
    if(!sql_query.exec())
    {
        qDebug()<<sql_query.lastError();
    }
    else
    {
        qDebug()<<"inserted!";
    }


    /*关闭注册窗口，打开登录窗口*/
    this->reg->regist.hide();
    this->show();
    return 0;
}

/*清除登录界面的所有显示*/
 int login::clear_log_ui()
 {
     this->ui->username->clear();
     this->ui->password->clear();
     return 0;
 }

 /*获取工程数据库接口*/
 QSqlDatabase get_acbook_db()
 {
     return g_db;
 }

 /*获取当前用户名*/
 QString *get_current_user()
 {
     return g_user;
 }

 /*打开数据库*/
 QSqlDatabase login::open_database()
 {
     this->dbfilename = this->username + ".db";
     g_db = QSqlDatabase::addDatabase("QSQLITE");
     g_db.setHostName("127.0.0.1");
     g_db.setDatabaseName(this->dbfilename);
     g_db.setUserName("admin");
     g_db.setPassword("123456");
     bool ret = g_db.open();
     if(ret == false)
     {
         qDebug() << "open database failed!";
     }
     check_user_pass_table(g_db);
     return g_db;
 }

 /*检查用户名密码的数据库表项*/
 bool login::check_user_pass_table(QSqlDatabase db)
 {
     QStringList tables = db.tables();
     QStringListIterator *itr = new QStringListIterator(tables);
     bool isFind = false;
     while (itr->hasNext())
     {
         QString tableNmae = itr->next().toLocal8Bit();
         if(tableNmae.compare(QString("userpass")) == 0)
         {
             isFind = true;
             break;
         }
     }
     if(isFind != true)
     {
         QSqlQuery sql_query;
         QString create_sql = QString("create table userpass (username varchar(30) primary key, password varchar(30))");
         sql_query.prepare(create_sql);
         if(!sql_query.exec())
         {
             qDebug() << "Error: Fail to create table." << sql_query.lastError();
             return false;
         }
         else
         {
             qDebug() << "Table created!";
         }
     }
     return true;
 }


 /*获取账单名称*/
 void login::get_bill_name(QSqlDatabase db)
 {
     QString bill_rec_table = this->username + "_bill";
     QStringList tables = db.tables();
     QStringList bill_tables;
     QSqlQuery sql_query;
     QStringListIterator *itr = new QStringListIterator(tables);
     bool isFind = false;
     /*查找数据库是否存在存放用户账单数据的表*/
     while (itr->hasNext())
     {
         QString tableNmae = itr->next().toLocal8Bit();
         if(tableNmae.compare(QString(bill_rec_table)) == 0)
         {
             isFind = true;
             break;
         }
     }

     /*没找到，创建表*/
     if(isFind != true)
     {
            /*创建表，用于存放用户的账单信息*/
         QString create_sql = QString("create table %1 (username varchar(30), billname text)").arg(bill_rec_table);
         sql_query.prepare(create_sql);
         if(!sql_query.exec())
         {
             qDebug() << "Error: Fail to create table." << sql_query.lastError();
             return;
         }
         else
         {
             qDebug() << "Table created!";
         }
     }


         /*查找当前用户是否创建了账单*/
         QString cmd = QString("select billname from %1 where username='%2'").arg(bill_rec_table,this->username);
         sql_query.prepare(cmd);

         if(!sql_query.exec())
         {
             qDebug() << "Error: Fail to exec table." << sql_query.lastError();
             return;
         }

         int count = 0;

         while(sql_query.next())
         {
             count++;
             this->bills << sql_query.value("billname").toString();
         }

         /*不存在一份账单，提醒用户创建*/
         qDebug() << "count = " << count;
         if(count == 0)
         {
             qDebug() << "123456789" << count;
             QMessageBox msgBox;
             msgBox.setText("还未创建账单！");
             msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
             msgBox.setButtonText (QMessageBox::Ok,QString("现在创建"));
             msgBox.setButtonText (QMessageBox::Cancel,QString("稍会创建"));
             msgBox.setDefaultButton(QMessageBox::Cancel);
             int ret = msgBox.exec();
             if(ret == QMessageBox::Cancel)
             {
                 return;
             }
             if(ret == QMessageBox::Ok)
             {
                   /*显示创建菜单的界面，进行账单创建*/
                   create_bill->create_bill.show();

             }
         }

        return;
 }


 void login::create_bill_ui_init(CREATE_BILL_TABLE *bill)
 {
     QVBoxLayout *layout = new QVBoxLayout(&bill->create_bill);
     bill->edit_label.setText("帐单名称:");
     bill->billplitter.addWidget(&bill->edit_label);
     bill->billplitter.addWidget(&bill->input_bill);
     layout->addWidget(&bill->billplitter);
     bill->confirm_btn.setText(tr("确认"));
     layout->addWidget(&bill->confirm_btn,Qt::AlignCenter);
 }


 int login::add_bill_to_table()
 {
    QString bill_name = this->create_bill->input_bill.text();
    QString bill_rec_table = this->username + "_bill";
    QString cmd = QString("select billname from %1 where username='%2'").arg(bill_rec_table,this->username);
    QSqlQuery sql_query;
    sql_query.prepare(cmd);
    if(!sql_query.exec(cmd))
    {
        qDebug() << "Error: Fail to exec select." << sql_query.lastError();
        return 0;
    }

    QString get_bname;

        while(sql_query.next())
        {
            get_bname = sql_query.value("billname").toString();
            qDebug() << "[" << get_bname << "]";
            if(get_bname.compare(bill_name) == 0)
            {
                QMessageBox::warning(this,"警告","该账单已经存在");
                goto EXIT;
            }
        }

        /*如果不存在，进行账单添加*/
        cmd = QString("insert into %1 values('%2','%3')").arg(bill_rec_table,this->username,bill_name);
        sql_query.prepare(cmd);
        if(!sql_query.exec(cmd))
        {
            qDebug() << "Error: Fail to exec insert." << sql_query.lastError();
            goto EXIT;
        }
        add_bill_table(bill_name);
        QMessageBox::information(this,"通知","创建成功");
        this->bills << bill_name;
        create_bill->create_bill.close();
        return 1;
EXIT:
    create_bill->create_bill.close();
    return 0;
 }

void login::add_bill_table(QString billname)
{
    QSqlQuery sql_query;
    QString create_table_name = this->username + "_" + billname;
    QString create_sql = QString("create table %1 (date text, type text primary key, consume_type text,value float, mark text)").arg(create_table_name);
    qDebug()<<create_sql;
    sql_query.prepare(create_sql);
    if(!sql_query.exec())
    {
        qDebug() << "Error: Fail to create table." << sql_query.lastError();
    }
    else
    {
        qDebug() << "Table created!";
    }

}
