/*用于解析存放消费类型的Json配置文件，并生成StringList类型的数据，供全局使用
功能只提供增删*/
#ifndef JSONCONFIG_H
#define JSONCONFIG_H
#include <QString>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>

class JsonConfig
{
public:
    JsonConfig();


    QStringList JsonFileParse(QString filename); /*解析Json配置文件*/
    int add_json_key_value(QString filename, QString value); /*添加一条配置*/
    int del_json_key_value(QString filename,QString key); /*删除一条配置*/
    QStringList consume_lists; /*存放解析得到的消费类型的变量*/
    int json_item_num;  /*记录Json配置文件的配置数量*/
private:
    QString jsonfile;

};

#endif // JSONCONFIG_H
