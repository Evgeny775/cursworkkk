#include "ticketmanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

TicketManager::TicketManager(QSqlDatabase &db) : db(db) {}

bool TicketManager::bookTicket(int userId, int scheduleId) {
    QSqlQuery q(db);
    q.prepare("INSERT INTO tickets (user_id, schedule_id) VALUES (?, ?)");
    q.addBindValue(userId);
    q.addBindValue(scheduleId);
    if (!q.exec()) return false;

    QSqlQuery q2(db);
    q2.prepare("UPDATE schedule SET seats_available = seats_available - 1 "
               "WHERE id = ?");
    q2.addBindValue(scheduleId);
    return q2.exec();
}

bool TicketManager::cancelTicket(int ticketId, int scheduleId) {
    db.transaction();

    QSqlQuery q(db);
    q.prepare("DELETE FROM tickets WHERE id = ?");
    q.addBindValue(ticketId);
    if (!q.exec()) {
        db.rollback();
        qDebug() << "Ошибка удаления билета:" << q.lastError().text();
        return false;
    }

    q.prepare("UPDATE schedule SET seats_available = seats_available + 1 WHERE id = ?");
    q.addBindValue(scheduleId);
    if (!q.exec()) {
        db.rollback();
        qDebug() << "Ошибка обновления мест:" << q.lastError().text();
        return false;
    }

    db.commit();
    return true;
}
