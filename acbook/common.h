#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlRecord>

void add_bill_to_table(QString table,QString billname);

#endif // COMMON_H
