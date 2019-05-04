#include "additem.h"
#include "ui_additem.h"
#include <QWidget>
#include <QCalendarWidget>
#include <QDate>
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include "login.h"

extern QStringList consume_type;
additem::additem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::additem)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("添加条目"));
    ui->date_select->setCalendarPopup(true);
    this->set_type_combox();
    ui->type->setCurrentIndex(0);
    QDoubleValidator *validator = new QDoubleValidator(0, 4294967296, 2, this);
    ui->value->setValidator(validator);
    ui->year_edit->setDisabled(true);
    ui->mon_edit->setDisabled(true);
    ui->day_edit->setDisabled(true);

    QString date = QDate::currentDate().toString(Qt::ISODate);

    QString year = date.split("-").at(0);
    QString mon = date.split("-").at(1);
    QString day = date.split("-").at(2);
    ui->year_edit->setText(year);
    ui->mon_edit->setText(mon);
    ui->day_edit->setText(day);
    ui->date_select->setDate(QDate::currentDate());
    connect(ui->date_select,SIGNAL(dateChanged(QDate)),this,SLOT(date_select_slot()));
    connect(ui->confirm_btn,SIGNAL(clicked()),this,SLOT(confirm_btn_slot()));

}

additem::~additem()
{
    delete ui;
}

int additem::date_select_slot()
{
    QString date = ui->date_select->date().toString(Qt::ISODate);
    QString year = date.split("-").at(0);
    QString mon = date.split("-").at(1);
    QString day = date.split("-").at(2);
    ui->year_edit->setText(year);
    ui->mon_edit->setText(mon);
    ui->day_edit->setText(day);
    qDebug() << date;
    return 0;
}


void additem::set_type_combox()
{
    int size = consume_type.size();
    for(int i = 0;i < size;i++)
    {
        ui->type->addItem(consume_type[i]);
    }
}

int additem::confirm_btn_slot()
{
    if(ui->value->text().isEmpty())
    {
        QMessageBox::warning(this,"通知","未输入金额！");
        return 0;
    }
    QString date = ui->date_select->date().toString(Qt::ISODate);
    QString ready_write_table = this->cur_bill_name;
    QSqlDatabase user_db = get_acbook_db();
    QStringList tables = user_db.tables();

    QStringListIterator *itr = new QStringListIterator(tables);
    bool isFind = false;
    QSqlQuery sql_query;
    while (itr->hasNext())
    {
        QString tableNmae = itr->next();
        qDebug() << tableNmae << "ddd";
        if(tableNmae.compare(ready_write_table) == 0)
        {
            qDebug("is exist!\n");
            isFind = true;
        }
    }

    /*检查数据表是否存在*/
    if(isFind == false)
    {
        QString create_sql = QString("create table %1 (date text, type text primary key,consume_type text,value float,mark text)").arg(ready_write_table);


        if(!sql_query.exec(create_sql))
        {
            qDebug()<< create_sql << sql_query.lastError();
        }
    }

    QString value = ui->value->text();
    QString type = ui->type->currentText();
    QString consume_type = "支出";
    QString mark = NULL;
    mark = ui->mark->text();
    QString old_mark = NULL;
    bool exist_flag = false;
    float old_value;
    float rec_value;
    QString new_mark = NULL;
    QString check_sql = QString("select value from %1 where date='%2' and type='%3'").arg(ready_write_table,date,type);
    QString mark_sql = QString("select mark from %1 where date='%2' and type='%3'").arg(ready_write_table,date,type);
    sql_query.prepare(check_sql);

    /*检测添加的条目是否存在*/
    if(!sql_query.exec(check_sql))
    {
        qDebug() << check_sql << sql_query.lastError();
        return 1;
    }
    /*查看当前金额*/
    while(sql_query.next())
    {
        if(sql_query.isActive() != 0)
        {
            exist_flag = true;
            old_value = sql_query.value(0).toFloat();

        }
    }
    /*查看备注并更新*/
    if(exist_flag == true)
    {
        if(!sql_query.exec(mark_sql))
        {
            qDebug() << sql_query.lastError();
            return 0;
        }
        while(sql_query.next())
        {
            if(sql_query.isActive() != 0)
            {
                if(!sql_query.value(0).isNull())
                {
                     old_mark = sql_query.value(0).toString();
                }
            }
        }
        if(mark != NULL)
        {
            if(old_mark.isNull())
            {
                new_mark = mark;
            }
            else
            {
                new_mark = old_mark + "^^^" + mark;
            }
        }
        rec_value = ui->value->text().toFloat() + old_value;
        QString value_str = QString("%1").arg(rec_value);
        QString update_sql = NULL;
        if(mark != NULL)
        {
            qDebug() << new_mark;
            update_sql = QString("update %1 set value='%2',mark='%3' where date='%4' and type='%5'").arg(ready_write_table,value_str,new_mark,date,type);
        }
        else
        {
            update_sql = QString("update %1 set value='%2' where date='%3' and type='%4'").arg(ready_write_table,value_str,date,type);
        }


        sql_query.prepare(update_sql);
        if(!sql_query.exec(update_sql))
        {
            qDebug()<< sql_query.lastError();
        }
        emit update_view();
        return 0;
    }

    QString insert_sql = QString("insert into %1 values('%2','%3','%4','%5','%6')").arg(ready_write_table,date,type,consume_type,value,mark);
    sql_query.prepare(insert_sql);

    qDebug() << insert_sql;
    if(!sql_query.exec(insert_sql))
    {
        qDebug()<< sql_query.lastError();
    }

    emit update_view();
    return 0;
}


void additem::update_record(additem *item,QSqlQuery sql_query,QString tablename,QString date,QString type)
{
    QString check_old_value= QString("SELECT %1 FROM %2 WHERE date='%3'").arg(type,tablename,date);
    QString get_old_mark= QString("SELECT mark FROM %1 WHERE date='%2'").arg(tablename,date);
    QString old_mark = NULL;
    QString new_mark;
    float old_value = 0;
    float rec_value = 0;
    sql_query.prepare(check_old_value);
    if(!sql_query.exec(check_old_value))
    {
        qDebug()<< sql_query.lastError();
    }
    while(sql_query.next())
    {
        if(sql_query.isActive() != 0)
        {
            old_value = sql_query.value(0).toFloat();

        }
    }

    sql_query.prepare(get_old_mark);
    if(!sql_query.exec(get_old_mark))
    {
        qDebug()<< sql_query.lastError();
    }
    while(sql_query.next())
    {
        if(sql_query.isActive() != 0)
        {
            if(!sql_query.value(0).isNull())
            {
                old_mark = sql_query.value(0).toString();
            }

        }
    }
    if(!item->ui->mark->text().isEmpty())
    {
        if(old_mark.isNull())
        {
            new_mark = item->ui->mark->text();
        }
        else
        {
            new_mark = old_mark + "\n" + item->ui->mark->text();
        }
    }
    rec_value = item->ui->value->text().toFloat() + old_value;
    QString value_str = QString("%1").arg(rec_value);
    QString update_sql = QString("update %1 set %2='%3' where date='%4'").arg(tablename,type,value_str,date);
    if(!item->ui->mark->text().isEmpty())
    {
        update_sql = QString("update %1 set %2='%3',mark='%4' where date='%5'").arg(tablename,type,value_str,new_mark,date);
    }
    sql_query.prepare(update_sql);
    if(!sql_query.exec(update_sql))
    {
        qDebug()<< sql_query.lastError();
    }
}


