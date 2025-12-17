#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QKeyEvent>
#include <QRegularExpressionValidator>
#include <QDate>
#include "databasemanager.h"
#include "authmanager.h"
#include "ticketmanager.h"
#include "user.h"

    QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_LoginButton_clicked();
    void on_registerButton_clicked();
    void on_goToRegisterButton_clicked();
    void showError(const QString &message);
    QSqlTableModel* initTable(QTableView *table, const QString &tableName);
    void updateDestinationList();
    void loadCities();
    void on_addUsersAdmin_clicked();
    void on_addCitiesAdmin_clicked();
    void on_addRoutesAdmin_clicked();
    void on_addScheduleAdmin_clicked();
    void on_addTicketsAdmin_clicked();
    void on_addRefundsAdmin_clicked();
    void on_deleteUsersAdmin_clicked();
    void on_usersAdmin_clicked(const QModelIndex &index);
    void on_citiesAdmin_clicked(const QModelIndex &index);
    void on_routesAdmin_clicked(const QModelIndex &index);
    void on_scheduleAdmin_clicked(const QModelIndex &index);
    void on_ticketsAdmin_clicked(const QModelIndex &index);
    void on_refundsAdmin_clicked(const QModelIndex &index);
    void on_deleteCitiesAdmin_clicked();
    void on_deleteRoutesAdmin_clicked();
    void on_deleteScheduleAdmin_clicked();
    void on_deleteTicketsAdmin_clicked();
    void on_deleteRefundsAdmin_clicked();
    void on_bookButton_clicked();
    void on_showRoutesButton_clicked();
    void loadUserTickets();
    void on_cancelTicketButton_clicked();
private:
    Ui::MainWindow *ui;
    DatabaseManager *dbManager;
    AuthManager *authManager;
    TicketManager *ticketManager;
    User *currentUser;
    QSqlTableModel *usersModel;
    QSqlTableModel *citiesModel;
    QSqlTableModel *routesModel;
    QSqlTableModel *scheduleModel;
    QSqlTableModel *ticketsModel;
    QSqlTableModel *refundsModel;
    QSqlTableModel *myTicketsModel;
    QSqlQueryModel *scheduleQueryModel;
    int usersRow;
    int citiesRow;
    int routesRow;
    int scheduleRow;
    int ticketsRow;
    int refundsRow;
protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // MAINWINDOW_H
