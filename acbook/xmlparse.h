#ifndef XMLPARSE_H
#define XMLPARSE_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QXmlStreamAttributes>
#include <QFile>
#include <QString>
#include <QStringList>

class XmlParse
{
public:
    XmlParse();
    XmlParse(QString filename);
    ~XmlParse();
    bool parse_file();
    QStringList consume_type_lists;

private:
    QXmlStreamReader reader;
    QString file_name;
    unsigned int read_consume_types();

};

#endif // XMLPARSE_H
