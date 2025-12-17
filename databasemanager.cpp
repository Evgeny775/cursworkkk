#include "databasemanager.h"
#include <QDebug>
#include <QSqlError>

DatabaseManager::DatabaseManager() : query(nullptr) {
    db = QSqlDatabase::addDatabase("QSQLITE");
}

DatabaseManager::~DatabaseManager() {
    if (query) {
        delete query;
    }
    closeDatabase();
}

bool DatabaseManager::openDatabase(const QString &dbName) {
    db.setDatabaseName(dbName);
    if (db.open()) {
        qDebug("Opened");
        query = new QSqlQuery(db);
        return true;
    } else {
        qDebug("Nah");
        return false;
    }
}

void DatabaseManager::closeDatabase() {
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseManager::createTables() {
    if (!query) return false;

    query->exec("PRAGMA foreign_keys = ON;");

    query->exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, phone TEXT UNIQUE NOT NULL, password_hash TEXT NOT NULL, role TEXT CHECK(role IN ('user', 'admin')) DEFAULT 'user', created_at TEXT DEFAULT (datetime('now')));");

    query->exec("CREATE TABLE IF NOT EXISTS cities (id INTEGER PRIMARY KEY AUTOINCREMENT,name TEXT NOT NULL);");

    query->exec("CREATE TABLE IF NOT EXISTS routes (id INTEGER PRIMARY KEY AUTOINCREMENT,city_from_id INTEGER NOT NULL,city_to_id INTEGER NOT NULL,distance_km INTEGER,FOREIGN KEY (city_from_id) REFERENCES cities(id) ON DELETE CASCADE,FOREIGN KEY (city_to_id) REFERENCES cities(id) ON DELETE CASCADE);");

    query->exec("CREATE TABLE IF NOT EXISTS schedule (id INTEGER PRIMARY KEY AUTOINCREMENT,route_id INTEGER NOT NULL,departure_time TEXT NOT NULL,arrival_time TEXT NOT NULL,price REAL NOT NULL,seats_total INTEGER NOT NULL,seats_available INTEGER NOT NULL,FOREIGN KEY (route_id) REFERENCES routes(id) ON DELETE CASCADE);");

    query->exec("CREATE TABLE IF NOT EXISTS tickets (id INTEGER PRIMARY KEY AUTOINCREMENT,user_id INTEGER NOT NULL,schedule_id INTEGER NOT NULL,purchase_time TEXT DEFAULT (datetime('now')),status TEXT CHECK(status IN ('active', 'refunded')) DEFAULT 'active',FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,FOREIGN KEY (schedule_id) REFERENCES schedule(id) ON DELETE CASCADE);");

    query->exec("CREATE TABLE IF NOT EXISTS refunds (id INTEGER PRIMARY KEY AUTOINCREMENT,ticket_id INTEGER NOT NULL,refund_time TEXT DEFAULT (datetime('now')),refund_amount REAL NOT NULL,FOREIGN KEY (ticket_id) REFERENCES tickets(id) ON DELETE CASCADE);");

    query->exec("CREATE VIEW IF NOT EXISTS tickets_view AS "
                "SELECT "
                "t.id AS ticket_id, "
                "c1.name AS city_from, "
                "c2.name AS city_to, "
                "s.departure_time, "
                "s.arrival_time, "
                "s.price, "
                "t.status, "
                "t.purchase_time, "
                "t.schedule_id, "
                "t.user_id "
                "FROM tickets t "
                "JOIN schedule s ON t.schedule_id = s.id "
                "JOIN routes r ON s.route_id = r.id "
                "JOIN cities c1 ON r.city_from_id = c1.id "
                "JOIN cities c2 ON r.city_to_id = c2.id;");

    return true;
}

QSqlDatabase& DatabaseManager::getDatabase() {
    return db;
}
