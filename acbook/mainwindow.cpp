#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDialog>
#include <QDebug>
#include <QLabel>
#include <QEventLoop>
#include <QDate>
#include <iostream>
#include <synchapi.h>

#include <QAbstractItemView>
#include <QCalendarWidget>
#include <QIcon>
#include <QTimer>
#include <QMessageBox>
#include "xmlparse.h"
#include "jsonconfig.h"

/*显示主窗口的标记*/
bool main_win_flag = false;
QString g_bill_name;
QStringList g_bills;
/*当前消费类型的string list*/
QStringList consume_type;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("记帐单"));
    this->setFixedSize(1200,750);
    ui->sqltable->setFixedSize(1050,590);
    this->reclog = NULL;

    /*先要登录才能显示主窗口*/
    main_win_flag = false;

    /*创建登录窗口，并显示*/
    login *log = new login();
    log->show();
    this->log = log;

    ui->sqltable->setEditTriggers(QAbstractItemView::DoubleClicked);

    /*检测是否登录的定时器*/
    timer = new QTimer(this);

    /*著串口菜单栏*/
    menubar = new QMenuBar(this);

    /*消费类型的json文件解析*/
    JsonConfig *jsonparse = new JsonConfig();
    consume_type = jsonparse->JsonFileParse("consume.json");

    /*菜单栏设置*/
    QMenu *display = new QMenu("查看");
    view_all_year = new QAction("查看全年",this);
    view_all_year->setCheckable(true);
    all_cost = new QAction("消费总额",this);
    all_cost->setCheckable(true);
    display->addAction(view_all_year);
    display->addAction(all_cost);

    QMenu *person = new QMenu("个性");

    person->addAction("主题");
    person->addAction("字体");

    QMenu *tool = new QMenu("工具");
    QAction *cal = new QAction("计算器");
    tool->addAction(cal);
    tool->addAction("记事本");
    tool->addAction("导出");

    QMenu *msetting = new QMenu("设置");


    QAction *normal_s = new QAction("常规设置");
    msetting->addAction(normal_s);
    QAction *account_s = new QAction("账户设置");
    msetting->addAction(account_s);
    QAction *type_s = new QAction("类型设置");
    msetting->addAction(type_s);

    menubar->addMenu(display);
    menubar->addMenu(person);
    menubar->addMenu(tool);
    menubar->addMenu(msetting);
    menubar->adjustSize();

    setting = new QPushButton("设置");

    /*添加设置类*/
   this->setting_ui = new Setting();


    /*时间下拉列表添加*/
    int y = 0;
    int m = 0;
    int len = 0;
    for(y=1990;y <= 2100;y++)
    {
        ui->year->addItem(QString::number(y));
    }

    QString insert_str;
    for(m = 1;m <= 12;m++)
    {
        len = QString::number(m).length();

        if(len == 1)
        {
            insert_str = "0" + QString::number(m);

        }
        else
        {
            insert_str = QString::number(m);
        }
        ui->month->addItem(insert_str);
    }

    /*获取当前时间*/
    QString date = QDate::currentDate().toString(Qt::ISODate);
    QString year = date.split("-").at(0);
    QString mon = date.split("-").at(1);
    QString day = date.split("-").at(2);

    /*设置下拉表为当前时间*/
    int yindex = year.toInt() - 1990;
    int mindex = mon.toInt() - 1;
    ui->year->setCurrentIndex(yindex);
    ui->month->setCurrentIndex(mindex);

    /*添加  添加记录的类*/
    this->item_rec = new additem();

    model = new QSqlQueryModel(ui->sqltable);

    ui->sqltable->setContextMenuPolicy(Qt::CustomContextMenu);  //少这句，右键没有任何反应的。
    create_right_menu();  //创建一个右键菜单
    connect(ui->sqltable,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(clicked_rightMenu(QPoint)));


    /*信号与槽设置*/
    connect(timer,SIGNAL(timeout()),this,SLOT(timer_handle_slot()));
    connect(ui->leavebtn,SIGNAL(clicked()),this,SLOT(leavebtn_click_slot()));
    connect(ui->add_item,SIGNAL(clicked()),this,SLOT(add_item_rec_slot()));
    connect(item_rec,SIGNAL(update_view()),this,SLOT(sql_table_view_show_slot()));
    connect(ui->year,SIGNAL(activated(int)),this,SLOT(sql_table_view_show_slot()));
    connect(ui->month,SIGNAL(activated(int)),this,SLOT(sql_table_view_show_slot()));
    connect(view_all_year,SIGNAL(triggered(bool)),this,SLOT(sql_table_view_show_slot()));
    connect(cal,SIGNAL(triggered(bool)),this,SLOT(call_calc()));
    connect(normal_s,SIGNAL(triggered(bool)),this,SLOT(normal_setting()));
    connect(deleteAction,SIGNAL(triggered(bool)),this,SLOT(del_one_rec()));
    connect(ui->cur_bill_box,SIGNAL(currentIndexChanged(QString)),this,SLOT(set_cur_bill()));
    connect(this->setting_ui,SIGNAL(signal_to_add_bill(QString)),this,SLOT(add_bill_to_table_slot(QString)));


    /*启动定时器*/
    timer->start(500);
}



MainWindow::~MainWindow()
{
    delete ui;
}


/************************************
 *brief: 退出登录按钮的槽函数
 *return:0表示成功-1表示失败
************************************/
int MainWindow::leavebtn_click_slot()
{
    QMessageBox msgBox;
    msgBox.setText("确认退出登录？");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setButtonText (QMessageBox::Ok,QString("退出"));
    msgBox.setButtonText (QMessageBox::Cancel,QString("取消"));
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if(ret == QMessageBox::Cancel)
    {
        return 1;
    }
    this->log->show();
    this->hide();
    main_win_flag = false;
    this->log->set_login_status(false);
    this->log->clear_log_ui();
    this->ui->cur_bill_box->clear();
    this->timer->start();
    return 0;
}

/************************************
 *brief: 检测是否登录的定时器槽函数
 *return:0表示成功，-1表示失败
************************************/
MainWindow::timer_handle_slot()
{

    if(get_login_status() == true && main_win_flag == false)
    {
        this->timer->stop();
        this->show();
        main_win_flag = true;
        /*添加设置类*/
        this->current_user = this->log->user;
        this->ui->current_user->setText(this->log->user);
        this->set_cur_bill_box();
        set_cur_bill();
        set_all_bills();
        this->setting_ui->get_bills(g_bills);
        sql_table_view_show_slot();
    }
    return 0;
}


/*显示添加条目的槽函数*/
int MainWindow::add_item_rec_slot()
{
    this->item_rec->set_type_combox();
    this->item_rec->show();
    return 0;
}


/*QTableView结合数据库显示记录*/
int MainWindow::sql_table_view_show_slot()
{
    QString current_year = ui->year->currentText();
    QString current_mon = ui->month->currentText();
    QString table_name = g_bill_name;
    QString start_select_date = current_year + "-" + current_mon + "-01";
    QString end_select_date = current_year + "-" + current_mon + "-31";



    qDebug() << start_select_date << end_select_date;
    QString select_sql;
    if(this->view_all_year->isChecked())
    {

        select_sql = QString("select * from %1 order by date").arg(table_name);
    }
    else
    {
        select_sql = QString("select * from %1 where date between '%2' and '%3' order by date").arg(table_name,start_select_date,end_select_date);
    }
    model->setQuery(select_sql);
    model->setHeaderData(0, Qt::Horizontal, tr("日期"));
    model->setHeaderData(1, Qt::Horizontal, tr("类型"));
    model->setHeaderData(2, Qt::Horizontal, tr("支出/收入"));
    model->setHeaderData(3, Qt::Horizontal, tr("金额"));
    model->setHeaderData(4, Qt::Horizontal, tr("备注"));

    ui->sqltable->setModel(model);
    ui->sqltable->setColumnWidth(0,120);
    ui->sqltable->setColumnWidth(1,120);
    ui->sqltable->setColumnWidth(2,120);
    ui->sqltable->setColumnWidth(3,120);
    ui->sqltable->setColumnWidth(4,300);

    ui->sqltable->verticalHeader()->hide();
    ui->sqltable->horizontalHeader()->setStretchLastSection(true);
    ui->sqltable->setSelectionBehavior(QAbstractItemView::SelectRows);
    return 0;
}

/*日期选择点击槽函数*/
void MainWindow::on_seclect_date_clicked()
{
    QCalendarWidget *cal = new QCalendarWidget();
    cal->setStyleSheet("QCalendarWidget { background-color: #fff;} \
                       QCalendarWidget QToolButton { \
                           height: 60px; color: white;font-size: 18px;font-weight: 700;\
                           icon-size: 20px, 20px;background-color: #3bafda;} \
                       QCalendarWidget QToolButton:hover { border: transparent;} \
                       QCalendarWidget QToolButton:pressed { border: transparent;background-color: #4fc1e9; } \
                              QCalendarWidget QToolButton#qt_calendar_prevmonth { \
                           qproperty-icon: url('/image/arrow-left.png');\
                       } \
                       QCalendarWidget QToolButton#qt_calendar_nextmonth { \
                           qproperty-icon: url('/image/arrow-right.png'); \
                       } \
                       QCalendarWidget QToolButton#qt_calendar_monthbutton::menu-indicator {\
                           background-color: #3bafda; \
                       } \
                       QCalendarWidget QToolButton#qt_calendar_monthbutton::menu-indicator:pressed { \
                           background-color: #4fc1e9; \
                       } \
                       QCalendarWidget QWidget#qt_calendar_calendarview { \
                           background-color: #fff; \
                           selection-color: #FFF;\
                           selection-background-color: #3bafda; \
                       } \
                       QCalendarWidget QWidget { \
                           alternate-background-color: #FFFFFF; \
                       } \
                       QCalendarWidget QWidget#qt_calendar_navigationbar { \
                           background-color: #3bafda; \
                       } \
                       QCalendarWidget QAbstractItemView { \
                           padding: 11px; \
                           font-size: 14px; \
                           outline: none; \
                           selection-color: #FFF; \
                           border: 1px solid #ccd1d9; \
                           border-bottom-left-radius: 4px; \
                           border-bottom-right-radius: 4px;\
                       }");
       cal->setWindowTitle("选择时间");

       cal->show();
}

/*调用计算器*/
void MainWindow::call_calc()
{
    system("calc.exe");
}

/*显示常规设置的界面*/
int MainWindow::normal_setting()
{
    this->setting_ui->show();
    return 0;
}

/*添加右击菜单*/
void MainWindow::create_right_menu()
{
    rightMenu = new QMenu;
    cutAction = new QAction("剪切",this);
    copyAction = new QAction("复制",this);
    pasteAction = new QAction("粘贴",this);
    deleteAction = new QAction("删除",this);

    rightMenu->addAction(cutAction);
    rightMenu->addAction(copyAction);
    rightMenu->addAction(pasteAction);
    rightMenu->addAction(deleteAction);

}

/*右击的槽函数*/
void MainWindow::clicked_rightMenu(const QPoint &pos)
{
    rightMenu->exec(QCursor::pos());
}

/*删除一条记录*/
void MainWindow::del_one_rec()
{
    int row = ui->sqltable->currentIndex().row();
    qDebug() << "deledt" << row;
    QAbstractItemModel *model = ui->sqltable->model();
    QItemSelectionModel *selections = ui->sqltable->selectionModel();
    QModelIndexList selected = selections->selectedIndexes();

    QModelIndex index =model->index(row,0);
    QString date = index.sibling(row,0).data().toString();
    QString type = index.sibling(row,1).data().toString();
    QString current_year = ui->year->currentText();
    QString table_name = this->log->user + "_" + current_year;
    QSqlQuery query;
    QString cmd;
    cmd =QString("delete from %1 where date='%2' and type='%3'").arg(table_name,date,type);
    query.exec(cmd);
    if(!query.isActive())
    {
        query.finish();
        sql_table_view_show_slot();
        return;
    }
    sql_table_view_show_slot();
    return;
 }


/*为tableview显示设置需要显示得到账单名称的槽函数*/
void MainWindow::set_cur_bill()
{
    g_bill_name = this->log->user + "_" + this->ui->cur_bill_box->currentText();
    this->item_rec->cur_bill_name = g_bill_name;
    sql_table_view_show_slot();
}

/*为g_bill设置值*/
void MainWindow::set_all_bills()
{
    g_bills = this->log->bills;
}


/*为当前用户新建一个新的账单*/
void MainWindow::add_bill_to_table_slot(QString bill_name)
{
    QString bill_rec_table = this->current_user + "_bill";
    QString cmd = QString("select billname from %1 where username='%2'").arg(bill_rec_table,this->current_user);
    QSqlQuery sql_query;
    sql_query.prepare(cmd);
    if(!sql_query.exec(cmd))
    {
        qDebug() << "Error: Fail to exec select." << sql_query.lastError();
        return;
    }

    QString get_bname;

        while(sql_query.next())
        {
            get_bname = sql_query.value("billname").toString();
            qDebug() << "[" << get_bname << "]";
            if(get_bname.compare(bill_name) == 0)
            {
                QMessageBox::warning(this,"警告","该账单已经存在");
                return;
            }
        }

        /*如果不存在，进行账单添加*/
        cmd = QString("insert into %1 values('%2','%3')").arg(bill_rec_table,this->current_user,bill_name);
        sql_query.prepare(cmd);
        if(!sql_query.exec(cmd))
        {
            qDebug() << "Error: Fail to exec insert." << sql_query.lastError();
            return;
        }

        QString create_table_name = this->current_user + "_" + bill_name;
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

        QMessageBox::information(this,"通知","创建成功");
        this->setting_ui->close_add_bill_ui();
        this->log->bills << bill_name;
        set_all_bills();
        this->setting_ui->bill_list_view_show();
        this->setting_ui->get_bills(g_bills);
        set_cur_bill_box();

        return;
}


/*设置ui的cur_bill_box显示及刷新*/
void MainWindow::set_cur_bill_box()
{
    int bill_count = this->log->bills.size();
    this->ui->cur_bill_box->clear();
    for(int i = 0;i < bill_count;i++)
    {
        this->ui->cur_bill_box->addItem(this->log->bills.at(i));
    }
}
