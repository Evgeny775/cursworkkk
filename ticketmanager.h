#ifndef TICKETMANAGER_H
#define TICKETMANAGER_H

#include <QSqlDatabase>

class TicketManager {
public:
    TicketManager(QSqlDatabase &db);

    bool bookTicket(int userId, int scheduleId);
    bool cancelTicket(int ticketId, int scheduleId);

private:
    QSqlDatabase &db;
};

#endif // TICKETMANAGER_H
