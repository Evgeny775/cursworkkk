#include "authmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

AuthManager::AuthManager(QSqlDatabase &db) : db(db) {}

QString AuthManager::hashPassword(const QString &password) {
    QByteArray hash = QCryptographicHash::hash(
        password.toUtf8(),
        QCryptographicHash::Sha256
        );
    QString hashedPassword = hash.toHex();
    return hashedPassword;
}

bool AuthManager::phoneExists(const QString &phone) {
    QSqlQuery query(db);
    query.prepare("SELECT id FROM users WHERE phone = ?");
    query.addBindValue(phone);
    if (!query.exec()) {
        qDebug() << "SQL error (phoneExists):" << query.lastError().text();
        return true;
    }
    return query.next();
}

bool AuthManager::addUser(const QString &phone, const QString &passwordHash, const QString &role) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO users (phone, password_hash, role) VALUES (?, ?, ?)");
    query.addBindValue(phone);
    query.addBindValue(passwordHash);
    query.addBindValue(role);
    if (!query.exec()) {
        qDebug() << "SQL error (add user):" << query.lastError().text();
        return false;
    }
    return true;
}

QString AuthManager::userValid(const QString &phone, const QString &passwordHash) {
    QSqlQuery query(db);
    query.prepare("SELECT password_hash, phone, role FROM users WHERE phone = ?");
    query.addBindValue(phone);
    qDebug() << "Phone input:" << phone;

    if (!query.exec()) {
        qDebug() << "Database error:" << query.lastError().text();
        return QString();
    }

    if (!query.next()) {
        return QString();
    }

    QString dphone = query.value("phone").toString();
    QString dpassword = query.value("password_hash").toString();
    QString drole = query.value("role").toString();

    if (phone != dphone) {
        return QString();
    }

    if (dpassword != passwordHash) {
        return QString();
    }

    return drole;
}

int AuthManager::getUserId(const QString &phone, const QString &hashedPassword) {
    QSqlQuery q(db);
    q.prepare("SELECT id FROM users WHERE phone=? AND password_hash=?");
    q.addBindValue(phone);
    q.addBindValue(hashedPassword);
    q.exec();
    if (q.next()) {
        return q.value("id").toInt();
    }
    return -1;
}
