#include "setting.h"
#include "ui_setting.h"
#include <QStringList>
#include <QMessageBox>
#include <QDebug>
#include <QTabBar>


/*存放消费类型的全局变量*/
extern QStringList consume_type;
Setting::Setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);
    ui->tabWidget->setTabPosition(QTabWidget::West);
    ui->tabWidget->tabBar()->setStyle(new CustomTabStyle);
    this->bill_view_model = new QStandardItemModel(this);
    int size = consume_type.size();
    for(int i = 0;i < size;i++)
    {
        ui->consume_box->addItem(consume_type[i]);
    }
    consume_json = new JsonConfig();
    consume_json->JsonFileParse("consume.json");
	
	/*添加消费类型的ui实例*/
    add_consume = new ADD_CONSUME;

	/*删除消费类型的ui实例*/
    del_consume = new DEL_CONSUME;
	
	/*初始化添加消费类型窗口*/
    add_consume_ui_widget_init();
	
	/*初始化删除消费类型的窗口*/
    del_consume_ui_widget_init();

    add_bill = new ADD_BILL;

    add_bill_ui_widget_init();
    bill_list_view_show();
	
	/*信号与槽 */
    connect(ui->add_type,SIGNAL(clicked()),this,SLOT(add_consume_ui_widget_show_slot()));
    connect(ui->del_type,SIGNAL(clicked()),this,SLOT(del_consume_ui_widget_show_slot()));
    connect(&add_consume->confirm_btn,SIGNAL(clicked()),this,SLOT(add_new_consume_type_into_json_btn_slot()));
    connect(&del_consume->confirm_btn,SIGNAL(clicked()),this,SLOT(del_consume_type_from_json_btn_slot()));
    connect(ui->add_bill,SIGNAL(clicked(bool)),this,SLOT(add_bill_ui_widget_show_slot()));
    connect(&add_bill->confirm_btn,SIGNAL(clicked(bool)),this,SLOT(add_bill_to_table_slot()));
    connect(ui->del_bill,SIGNAL(clicked(bool)),this,SLOT(del_bill_from_table_slot()));
}


/*析构函数*/
Setting::~Setting()
{
    delete ui;
}

void Setting::add_consume_ui_widget_init()
{
    QVBoxLayout *layout = new QVBoxLayout(&add_consume->add_consume);
    add_consume->cur_consume_label.setText(tr("当前类型:"));
    add_consume->cur_consume_splitter.addWidget(&add_consume->cur_consume_label);
    add_consume->cur_consume_splitter.addWidget(&add_consume->cur_consume_combobox);

    layout->addWidget(&add_consume->cur_consume_splitter);
    layout->addSpacerItem(new QSpacerItem(20,20));

    add_consume->input_label.setText(tr("添加类型:"));
    add_consume->input_consume_splitter.addWidget(&add_consume->input_label);
    add_consume->input_consume_splitter.addWidget(&add_consume->add_consume_edit);
    layout->addWidget(&add_consume->input_consume_splitter);
    layout->addSpacerItem(new QSpacerItem(20,20));
    add_consume->confirm_btn.setText(tr("确认"));
    layout->addWidget(&add_consume->confirm_btn,0,Qt::AlignCenter);
    add_consume->add_consume.setWindowTitle(tr("添加消费类型"));
}

void Setting::add_consume_combobox_update()
{
    add_consume->cur_consume_combobox.clear();
    for(int i = 0;i < consume_type.size();i++)
    {
        add_consume->cur_consume_combobox.addItem(consume_type[i]);
    }
}

void Setting::add_consume_ui_widget_show_slot()
{
    add_consume_combobox_update();
    add_consume->add_consume.show();
}


void Setting::add_new_consume_type_into_json_btn_slot()
{
    if(add_consume->add_consume_edit.text().isEmpty())
    {
        QMessageBox::warning(this,"警告","输入为空");
        return;
    }
    for(int i = 0; i < consume_type.size();i++)
    {
         qDebug() << "[" << consume_type[i] << "|" << add_consume->add_consume_edit.text() <<"]";
        if(add_consume->add_consume_edit.text().compare(consume_type[i]) == 0)
        {
            QMessageBox::warning(this,"警告","该类型已存在");
            add_consume->add_consume_edit.clear();
            return;
        }
    }
    QMessageBox::information(this,"通知","添加成功");
    consume_type << add_consume->add_consume_edit.text();
    consume_json->add_json_key_value("consume.json",add_consume->add_consume_edit.text());
    consume_json->json_item_num = consume_json->json_item_num + 1;
    del_consume_combobox_update();
    add_consume_combobox_update();
    add_consume->add_consume.update();
    del_consume->del_consume.update();
    ui->consume_box->addItem(add_consume->add_consume_edit.text());
    ui->consume_box->update();
    return;
}

void Setting::del_consume_ui_widget_init()
{
    QVBoxLayout *layout = new QVBoxLayout(&del_consume->del_consume);
    del_consume->cur_consume_label.setText(tr("选择类型:"));
    del_consume->cur_consume_splitter.addWidget(&del_consume->cur_consume_label);
    del_consume->cur_consume_splitter.addWidget(&del_consume->cur_consume_combobox);

    layout->addWidget(&del_consume->cur_consume_splitter);
    layout->addSpacerItem(new QSpacerItem(20,20));
    del_consume->confirm_btn.setText(tr("确认"));
    layout->addWidget(&del_consume->confirm_btn,0,Qt::AlignCenter);
    del_consume->del_consume.setWindowTitle(tr("删除消费类型"));
}


void Setting::del_consume_combobox_update()
{
    del_consume->cur_consume_combobox.clear();
    for(int i = 0;i < consume_type.size();i++)
    {

        del_consume->cur_consume_combobox.addItem(consume_type[i]);
    }
}

void Setting::del_consume_ui_widget_show_slot()
{
    del_consume_combobox_update();
    del_consume->del_consume.show();
}

void Setting::del_consume_type_from_json_btn_slot()
{

    QString select_type = del_consume->cur_consume_combobox.currentText();
    int index = del_consume->cur_consume_combobox.currentIndex();
    consume_json->del_json_key_value("consume.json",select_type);
    int size = consume_type.size();
    for(int i = 0; i < size; i++)
    {
        if(select_type.compare(consume_type[i]))
        {
            consume_type.removeAt(i);
            consume_type.removeAt(i);
            QMessageBox::information(this,"通知","删除成功");
            break;
        }
    }

    consume_json->json_item_num = consume_json->json_item_num - 1;
    del_consume->del_consume.close();
    del_consume_combobox_update();
    add_consume_combobox_update();
    add_consume->add_consume.update();
    del_consume->del_consume.update();
    ui->consume_box->removeItem(index);
    ui->consume_box->update();
    return;
}


void Setting::bill_list_view_show()
{
    int count = this->bills.count();
    this->bill_view_model->clear();
    for(int i = 0; i < count; i++)
    {
        QStandardItem *item = new QStandardItem(this->bills.at(i));
        this->bill_view_model->appendRow(item);
    }
    ui->bill_list_view->setModel(this->bill_view_model);
}


void Setting::get_bills(QStringList bills)
{
    this->bills = bills;
    bill_list_view_show();
}

void Setting::add_bill_ui_widget_show_slot()
{
    add_bill->add_bill.show();
}

void Setting::add_bill_ui_widget_init()
{
    QVBoxLayout *layout = new QVBoxLayout(&add_bill->add_bill);
    add_bill->label.setText(tr("新建账单:"));
    add_bill->splitter.addWidget(&add_bill->label);
    add_bill->splitter.addWidget(&add_bill->add_bill_edit);

    layout->addWidget(&add_bill->splitter);
    layout->addSpacerItem(new QSpacerItem(20,20));
    add_bill->confirm_btn.setText(tr("确认"));
    layout->addWidget(&add_bill->confirm_btn,0,Qt::AlignCenter);
    add_bill->add_bill.setWindowTitle(tr("新建账单"));
}

void Setting::add_bill_to_table_slot()
{
    emit signal_to_add_bill(add_bill->add_bill_edit.text());
}

void Setting::close_add_bill_ui()
{
    add_bill->add_bill.close();
}

void Setting::del_bill_from_table_slot()
{
    QModelIndex cur_bill_name = ui->bill_list_view->currentIndex();
}
