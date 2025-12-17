#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QCryptographicHash>

class AuthManager {
public:
    AuthManager(QSqlDatabase &db);

    QString hashPassword(const QString &password);
    bool phoneExists(const QString &phone);
    bool addUser(const QString &phone, const QString &passwordHash, const QString &role);
    QString userValid(const QString &phone, const QString &passwordHash);
    int getUserId(const QString &phone, const QString &hashedPassword);

private:
    QSqlDatabase &db;
};

#endif // AUTHMANAGER_H
