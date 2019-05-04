/*additem类，用于添加一条消费记录，
 * 添加的条目格式是时间--类型--收入/支出--金额--备注
*/
#ifndef ADDITEM_H
#define ADDITEM_H

#include <QWidget>
#include <QString>
#include <synchapi.h>
#include <QStringList>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlRecord>
#include <Qtsql/QSqlQuery>
#include <QStringList>
#include <QStringListIterator>

/*记录类型枚举*/
typedef enum item_type
{
    ITEM_FOOD,
    ITEM_COM,
    ITEM_TRANS,
    ITEM_SHOP,
    ITEM_HEAL,
    ITEM_REAL,
    ITEM_ENTER,
    ITEM_OTHEAR
}ITEM_TYPE;

#define TYPE_FOOD "food"
#define TYPE_COM "communication"
#define TYPE_TRANS "transport"
#define TYPE_SHOP "shopping"
#define TYPE_HEAL "health"
#define TYPE_REAL "relationship"
#define TYPE_ENTER "entertainment"
#define TYPE_OTHER "other"
#define TYPE_MARK "mark"


/*additem添加记录项的类*/
namespace Ui {
class additem;
}

class additem : public QWidget
{
    Q_OBJECT

public:
    explicit additem(QWidget *parent = 0);
    ~additem();
private:
    Ui::additem *ui;
    ITEM_TYPE select_type;

private slots:
    int date_select_slot();
    int confirm_btn_slot();

public:
    /*保存当前登录用户*/
    QString user;
    QString cur_bill_name;
    void set_type_combox(); /*设置combobox显示*/
    /*更新显示*/
    void update_record(additem *item, QSqlQuery sql_query, QString tablename, QString date, QString type);
signals:

    void update_view(); /*用于在田间或者修改记录时，发射信号，通知更新显示*/

};



#endif // ADDITEM_H
