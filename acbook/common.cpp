#include "common.h"

void add_bill_to_table(QString table,QString billname)
{
    QString cmd = QString("select billname from %1 where username='%2'").arg(table,this->username);
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
            if(get_bname.compare(billname) == 0)
            {
                QMessageBox::warning(this,"警告","该账单已经存在");
                goto EXIT;
            }
        }

        /*如果不存在，进行账单添加*/
        cmd = QString("insert into %1 values('%2','%3')").arg(bill_rec_table,this->username,billname);
        sql_query.prepare(cmd);
        if(!sql_query.exec(cmd))
        {
            qDebug() << "Error: Fail to exec insert." << sql_query.lastError();
            goto EXIT;
        }
        add_bill_table(billname);
        QMessageBox::information(this,"通知","创建成功");

        create_bill->create_bill.close();
        return 1;
EXIT:
    create_bill->create_bill.close();
    return 0;
}
