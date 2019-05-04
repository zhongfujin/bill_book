#include "xmlparse.h"
#include <iostream>
#include <QDebug>
#include <QDateTime>
#include <synchapi.h>

XmlParse::XmlParse()
{

}

XmlParse::XmlParse(QString filename)
{
    file_name = filename;
}

bool XmlParse::parse_file()
{
    QFile file(file_name);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "can't open file " << file_name;
        return false;
    }
    reader.setDevice(&file);
    reader.readNext();
    while(!reader.atEnd())
    {
        qDebug() << "start parse!";
        if(reader.isStartElement())
        {
            if(reader.name() == "consume")
            {
                qDebug() << "start parse!";
                if(read_consume_types() == 1)
                {
                    file.close();
                    qDebug() << "return!";
                    return true;
                }
                else
                {
                    qDebug() << "else!";
                    file.close();
                    return false;
                }
            }
            else
            {

                qDebug() << "error con";
                file.close();
                return false;
            }
        }
        else
        {
            reader.readNext();
        }
    }

    return true;
}

unsigned int XmlParse::read_consume_types()
{
    QXmlStreamAttributes attributes;
    reader.readNext();
    while(!(reader.atEnd()))
    {
        if(reader.isEndElement())
        {
            reader.readNext();
            break;
        }

        if(reader.isStartElement())
        {
            if(reader.name() == "type")
            {
                attributes = reader.attributes();
                if(attributes.hasAttribute("name"))
                {
                    qDebug() << "[" << attributes.value("name").toString() << "]";
                    Sleep(100000);
                    consume_type_lists << attributes.value("name").toString();

                }
            }
        }
        else
        {
            qDebug() << "[" << "nenenenne" << "]";
            reader.readNext();
        }
    }

    int size = consume_type_lists.size();
    for(int i = 0;i < size;i++)
    {
        qDebug() << consume_type_lists[i];
    }
    return 1;
}

XmlParse::~XmlParse()
{

}
