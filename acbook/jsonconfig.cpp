#include "jsonconfig.h"
#include <QDebug>
#include <jsonconfig.h>

JsonConfig::JsonConfig()
{

}

QStringList JsonConfig::JsonFileParse(QString filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString value = file.readAll();
    file.close();

    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
    if(!(parseJsonErr.error == QJsonParseError::NoError))
    {
        qDebug() << "解析json文件错误！";
        return consume_lists;
    }

    QJsonObject jsonObject = document.object();
    if(jsonObject.contains(QStringLiteral("type")))
    {
         QJsonValue arrayValue = jsonObject.value(QStringLiteral("type"));
         if(arrayValue.isArray())
         {
             QJsonArray array = arrayValue.toArray();
             for(int i = 0;i < array.size();i++)
             {
                 QJsonValue item = array.at(i);
                 QJsonObject obj = item.toObject();
                 int id = obj["id"].toInt();
                 QString name = obj["name"].toString();
                 consume_lists << name.trimmed();
             }
         }
    }
    this->json_item_num = consume_lists.size();
    for(int i = 0;i < json_item_num;i++)
    {
        qDebug() << "|"<<consume_lists[i]<<"|";
    }
    return consume_lists;
}


int JsonConfig::add_json_key_value(QString filename,QString value)
{
    QFile file(filename);
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    QString context = file.readAll();

    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(context.toUtf8(),&parseJsonErr);
    if(!(parseJsonErr.error == QJsonParseError::NoError))
    {
        qDebug() << "解析json文件错误！";
        return -1;
    }
    int size = this->json_item_num;
    QJsonObject jsonObject = document.object();
    QJsonArray array;
    QJsonArray jsonArray;
    QJsonDocument jsonDoc;
    if(jsonObject.contains(QStringLiteral("type")))
    {
        QJsonValue arrayValue = jsonObject.value(QStringLiteral("type"));
        if(arrayValue.isArray())
        {
            array = arrayValue.toArray();
            for(int i = 0;i < array.size();i++)
            {
                QJsonValue item = array.at(i);
                QJsonObject obj = item.toObject();
                jsonArray.append(obj);
            }
        }
    }

    QJsonObject add_obj;
    add_obj.insert("id",size + 1);
    add_obj.insert("name",value);
    jsonArray.append(add_obj);
    jsonDoc.setArray(jsonArray);
    QByteArray byteArray = jsonDoc.toJson(QJsonDocument::Compact);
    QString strJson(byteArray);
    file.resize(0);
    qDebug() <<"haha" << strJson;
    file.write("{\n");
    file.write("    \"type\":");
    file.write(jsonDoc.toJson());
    file.write("}");
    file.close();
    return 1;
}
int JsonConfig::del_json_key_value(QString filename,QString key)
{
    QFile file(filename);
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    QString context = file.readAll();

    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(context.toUtf8(),&parseJsonErr);
    if(!(parseJsonErr.error == QJsonParseError::NoError))
    {
        qDebug() << "解析json文件错误！";
        return -1;
    }

    QJsonObject jsonObject = document.object();
    QJsonArray array;
    QJsonArray jsonArray;
    QJsonDocument jsonDoc;
    if(jsonObject.contains(QStringLiteral("type")))
    {
        QJsonValue arrayValue = jsonObject.value(QStringLiteral("type"));
        if(arrayValue.isArray())
        {
            array = arrayValue.toArray();
            for(int i = 0;i < array.size();i++)
            {
                QJsonValue item = array.at(i);
                QJsonObject obj = item.toObject();
                if(obj["name"].toString().compare(&key) != 0)
                {
                    jsonArray.append(obj);
                }

            }
        }
    }


    jsonDoc.setArray(jsonArray);
    file.resize(0);
    file.write("{\n");
    file.write("    \"type\":");
    file.write(jsonDoc.toJson());
    file.write("}");
    file.close();
    return 1;
}
