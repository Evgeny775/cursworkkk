#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QTableView>
#include <QSqlError>
#include <QSqlRecord>

class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();

    bool openDatabase(const QString &dbName);
    void closeDatabase();
    bool createTables();
    QSqlDatabase& getDatabase();

private:
    QSqlDatabase db;
    QSqlQuery *query;
};

#endif // DATABASEMANAGER_H
