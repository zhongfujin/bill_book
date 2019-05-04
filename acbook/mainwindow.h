#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include <QTextCodec>
#include <QStringList>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlRecord>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QStandardItemModel>
#include <QSqlQueryModel>
#include <QPoint>
#include "login.h"
#include "additem.h"
#include "setting.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void create_right_menu();
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    login *reclog;
    login *log;
    additem *item_rec;
    Setting *setting_ui;
    QString current_user;
    QMenuBar *menubar;
    QAction *view_all_year;
    QAction *all_cost;
    QAction *set_path;
    QPushButton *setting;
    QMenu   menu;
    QSqlQueryModel *model;


    QMenu *rightMenu;  //右键菜单
    QAction *cutAction;  //剪切
    QAction *copyAction;  //复制
    QAction *pasteAction;  //粘贴
    QAction *deleteAction;  //删除

    void set_cur_bill_box();
private slots:
    int leavebtn_click_slot();
    int timer_handle_slot();
    int add_item_rec_slot();
    int sql_table_view_show_slot();
    void on_seclect_date_clicked();
    int normal_setting();
    void call_calc();
    void clicked_rightMenu(const QPoint &pos);
    void del_one_rec();
    void set_cur_bill();
    void set_all_bills();
    void add_bill_to_table_slot(QString bill_name);

public:
    QThread thread;
};

#endif // MAINWINDOW_H
