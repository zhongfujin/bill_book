#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSplitter>
#include <QPushButton>
#include <QComboBox>
#include <QSplitter>
#include <QSpacerItem>
#include <QProxyStyle>
#include <QPainter>
#include <QStandardItemModel>
#include <QStandardItem>
#include <jsonconfig.h>

/*添加消费类型的结构体*/
typedef struct tag_add_consume
{
    QWidget add_consume;
    QVBoxLayout layout;
    QLabel cur_consume_label;
    QComboBox cur_consume_combobox;
    QSplitter cur_consume_splitter;
    QLabel input_label;
    QLineEdit add_consume_edit;
    QSplitter input_consume_splitter;
    QPushButton confirm_btn;
} ADD_CONSUME;

/*删除消费类型的结构体*/
typedef struct tag_del_consume
{
    QWidget del_consume;
    QVBoxLayout layout;
    QLabel cur_consume_label;
    QComboBox cur_consume_combobox;
    QSplitter cur_consume_splitter;
    QLabel label;
    QPushButton confirm_btn;
} DEL_CONSUME;


/*添加新账单*/
typedef struct tag_add_bill
{
    QWidget add_bill;
    QLabel label;
    QLineEdit add_bill_edit;
    QSplitter splitter;
    QPushButton confirm_btn;

}ADD_BILL;

namespace Ui {
class Setting;
}

class Setting : public QWidget
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = 0);

    ~Setting();

    /*bill setting*/
    void get_bills(QStringList bills);
    void bill_list_view_show();
    void close_add_bill_ui();

private:
    Ui::Setting *ui;

    /*normal setting*/
    ADD_CONSUME *add_consume;
    DEL_CONSUME *del_consume;
    JsonConfig *consume_json;
    void add_consume_combobox_update();
    void add_consume_ui_widget_init();
    void del_consume_combobox_update();
    void del_consume_ui_widget_init();

    /*bill setting*/
    ADD_BILL *add_bill;
    QStringList bills;
    QStandardItemModel *bill_view_model;
    void add_bill_ui_widget_init();



private slots:
    /*normal setting*/
    void add_consume_ui_widget_show_slot();
    void add_new_consume_type_into_json_btn_slot();
    void del_consume_ui_widget_show_slot();
    void del_consume_type_from_json_btn_slot();

    /*bill setting*/
    void add_bill_ui_widget_show_slot();
    void add_bill_to_table_slot();
    void del_bill_from_table_slot();

signals:
    void signal_to_add_bill(QString bill_name);
};


class CustomTabStyle : public QProxyStyle
{
public:
    /* sizeFromContents
     *  用于设置Tab标签大小
     * 1.获取原标签大小
     * 2.宽高切换
     * 3.强制宽高
     * 4.return
     * */
    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
        const QSize &size, const QWidget *widget) const
    {
        QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
        if (type == QStyle::CT_TabBarTab) {
            s.transpose();
            s.rwidth() = 120; // 设置每个tabBar中item的大小
            s.rheight() = 44;
        }
        return s;
    }

    /* drawControl
     *  画控件
     * 1.过滤CE_TabBarTabLabel
     * 2.选择状态下的背景绘制
     *      a.获取Rect
     *      b.保存旧的画笔
     *      c.设置新画笔
     *      d.画 Rect
     *      e.恢复旧的画笔
     * 3.设定Text对齐及选中/非选中状态下的Text颜色
     * 4.过滤CE_TabBarTab，对其进行绘制
     * */
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
    {
        if (element == CE_TabBarTabLabel) {
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option))
            {
                QRect allRect = tab->rect;

                if (tab->state & QStyle::State_Selected) {
                    painter->save();
                    painter->setPen(0x89cfff);
                    painter->setBrush(QBrush(0x89cfff));
                    QRect nrct = allRect.adjusted(6, 6, -6, -6);
                    painter->drawRect(allRect.adjusted(6, 6, -6, -6));
                    painter->restore();
                }
                QTextOption option;
                option.setAlignment(Qt::AlignCenter);
                if (tab->state & QStyle::State_Selected) {
                    painter->setPen(0xf8fcff);
                }
                else {
                    painter->setPen(0x5d5d5d);
                }

                painter->drawText(allRect, tab->text, option);
                return;
            }
        }

        if (element == CE_TabBarTab) {
            QProxyStyle::drawControl(element, option, painter, widget);
        }
    }
};


#endif // SETTING_H
