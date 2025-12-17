#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>

    enum Pages { PAGE_LOGIN = 0, MAIN_PAGE = 1, REGISTER_PAGE = 2, ADMIN_PAGE = 3};

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->PhoneInput, &QLineEdit::returnPressed, this, &MainWindow::on_LoginButton_clicked);
    connect(ui->PasswordInput, &QLineEdit::returnPressed, this, &MainWindow::on_LoginButton_clicked);

    ui->PasswordInput->setValidator(new QRegularExpressionValidator(
        QRegularExpression("^[A-Za-z0-9]{4,}$"), this));
    ui->InputName->setValidator(new QRegularExpressionValidator(
        QRegularExpression("^[А-Яа-я]{2,}$"), this));
    ui->PasswordInputReg->setValidator(new QRegularExpressionValidator(
        QRegularExpression("^[A-Za-z0-9]{6,}$"), this));
    ui->PasswordInputRepeatReg->setValidator(new QRegularExpressionValidator(
        QRegularExpression("^[A-Za-z0-9]{6,}$"), this));

    ui->stackedWidget->setCurrentIndex(0);

    dbManager = new DatabaseManager();
    dbManager->openDatabase("./testDB.db");
    dbManager->createTables();

    authManager = new AuthManager(dbManager->getDatabase());
    ticketManager = new TicketManager(dbManager->getDatabase());
    currentUser = new User();

    loadCities();
    ui->inputDate->setDate(QDate::currentDate());
    ui->inputDate->setCalendarPopup(true);

    scheduleQueryModel = new QSqlQueryModel(this);
    ui->bookTable->setModel(scheduleQueryModel);
    ui->bookTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    usersModel = initTable(ui->usersAdmin,"users");
    citiesModel =initTable(ui->citiesAdmin,"cities");
    routesModel = initTable(ui->routesAdmin,"routes");
    scheduleModel = initTable(ui->scheduleAdmin,"schedule");
    ticketsModel = initTable(ui->ticketsAdmin,"tickets");
    refundsModel = initTable(ui->refundsAdmin,"refunds");

    qDebug() << authManager->hashPassword("1111");
}

MainWindow::~MainWindow()
{
    delete ui;
    usersModel->submitAll();
    citiesModel->submitAll();
    routesModel->submitAll();
    scheduleModel->submitAll();
    ticketsModel->submitAll();
    refundsModel->submitAll();
    delete dbManager;
    delete authManager;
    delete ticketManager;
    delete currentUser;
}

QSqlTableModel* MainWindow::initTable(QTableView *table, const QString &tableName) {
    QSqlTableModel *model = new QSqlTableModel(this, dbManager->getDatabase());
    model->setTable(tableName);
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    model->select();
    table->setModel(model);
    return model;
}

void MainWindow::showError(const QString &message)
{
    QMessageBox* msg = new QMessageBox(this);
    msg->setWindowTitle("Ошибка");
    msg->setText(message);
    msg->setIcon(QMessageBox::Warning);
    msg->setStandardButtons(QMessageBox::Ok);
    msg->setAttribute(Qt::WA_DeleteOnClose);
    msg->setStyleSheet(
        "QMessageBox {"
        " background-color: #1f1f1f;"
        " color: #ffffff;"
        "}"
        "QMessageBox QLabel {"
        " color: #e0e0e0;"
        " font-size: 12pt;"
        "}"
        "QMessageBox QPushButton {"
        " background-color: #2b2b2b;"
        " color: #ffffff;"
        " border-radius: 10px;"
        " padding: 6px 14px;"
        " border: 2px solid #4a4a4a;}"
        "QMessageBox QPushButton:hover {background-color: #3a3a3a;border-color: #6a6a6a;}"
        "QMessageBox QPushButton:pressed {background-color: #222222;}");
    msg->exec();
}

void MainWindow::loadCities()
{
    QSqlQuery q(dbManager->getDatabase());
    q.exec("SELECT id, name FROM cities ORDER BY name");
    while (q.next()) {
        int id = q.value(0).toInt();
        QString name = q.value(1).toString();
        ui->inputFrom->addItem(name, id);
        ui->inputTo->addItem(name, id);
    }
    connect(ui->inputFrom, &QComboBox::currentIndexChanged,
            this, &MainWindow::updateDestinationList);
}

void MainWindow::updateDestinationList()
{
    int fromId = ui->inputFrom->currentData().toInt();
    ui->inputTo->blockSignals(true);
    ui->inputTo->clear();
    QSqlQuery q(dbManager->getDatabase());
    q.exec("SELECT id, name FROM cities ORDER BY name");
    while (q.next()) {
        int id = q.value(0).toInt();
        QString name = q.value(1).toString();
        if (id != fromId) {
            ui->inputTo->addItem(name, id);
        }
    }
    ui->inputTo->blockSignals(false);
}

void MainWindow::on_LoginButton_clicked()
{
    if (!ui->PhoneInput->hasAcceptableInput()) {
        showError("Введите корректный номер телефона!");
        return;
    }

    if (!ui->PasswordInput->hasAcceptableInput()) {
        showError("Введите корректный пароль!");
        return;
    }

    QString hashedPassword = authManager->hashPassword(ui->PasswordInput->text());
    QString role = authManager->userValid(ui->PhoneInput->text(), hashedPassword);

    if(role == "admin"){
        ui->stackedWidget->setCurrentIndex(ADMIN_PAGE);
        return;
    }

    if(role == "user"){
        int userId = authManager->getUserId(ui->PhoneInput->text(), hashedPassword);
        currentUser->setId(userId);
        currentUser->setPhone(ui->PhoneInput->text());
        currentUser->setRole(role);
        loadUserTickets();
        ui->stackedWidget->setCurrentIndex(MAIN_PAGE);
        return;
    }

    if(role.isEmpty()){
        showError("Неверный номер телефона или пароль!");
    }
}

void MainWindow::on_registerButton_clicked()
{
    if (!ui->InputName->hasAcceptableInput()) {
        showError("Введите корректное имя!");
        return;
    }

    if (!ui->PhoneInputReg->hasAcceptableInput()) {
        showError("Введите корректный номер телефона!");
        return;
    }

    if(authManager->phoneExists(ui->PhoneInputReg->text())){
        showError("Номер телефона уже зарегистрирован!");
        return;
    }

    if (!ui->PasswordInputReg->hasAcceptableInput()) {
        if(ui->PasswordInputReg->text().length()<6){
            showError("Пароль долже содержать минимум 6 символов!");
            return;
        }
        showError("Введите корректный пароль!");
        return;
    }

    if (ui->PasswordInputReg->text() != ui->PasswordInputRepeatReg->text()) {
        showError("Пароли должны совпадать!");
        return;
    }

    QString password = ui->PasswordInputReg->text();
    QString hashedPassword = authManager->hashPassword(password);
    QString phone = ui->PhoneInputReg->text();
    QString role = "user";

    authManager->addUser(phone, hashedPassword,role);
    int userId = authManager->getUserId(phone, hashedPassword);
    currentUser->setId(userId);
    currentUser->setPhone(phone);
    currentUser->setRole(role);
    ui->stackedWidget->setCurrentIndex(MAIN_PAGE);
}

void MainWindow::on_goToRegisterButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(REGISTER_PAGE);
}

void MainWindow::on_addUsersAdmin_clicked()
{
    usersModel->insertRow(usersModel->rowCount());
    usersModel->submitAll();
}

void MainWindow::on_addCitiesAdmin_clicked()
{
    citiesModel->insertRow(citiesModel->rowCount());
    citiesModel->submitAll();
}

void MainWindow::on_addRoutesAdmin_clicked()
{
    routesModel->insertRow(routesModel->rowCount());
    routesModel->submitAll();
}

void MainWindow::on_addScheduleAdmin_clicked()
{
    scheduleModel->insertRow(scheduleModel->rowCount());
    scheduleModel->submitAll();
}

void MainWindow::on_addTicketsAdmin_clicked()
{
    ticketsModel->insertRow(ticketsModel->rowCount());
    ticketsModel->submitAll();
}

void MainWindow::on_addRefundsAdmin_clicked()
{
    refundsModel->insertRow(refundsModel->rowCount());
    refundsModel->submitAll();
}

void MainWindow::on_usersAdmin_clicked(const QModelIndex &index)
{
    usersRow = index.row();
}

void MainWindow::on_citiesAdmin_clicked(const QModelIndex &index)
{
    citiesRow = index.row();
}

void MainWindow::on_routesAdmin_clicked(const QModelIndex &index)
{
    routesRow = index.row();
}

void MainWindow::on_scheduleAdmin_clicked(const QModelIndex &index)
{
    scheduleRow = index.row();
}

void MainWindow::on_ticketsAdmin_clicked(const QModelIndex &index)
{
    ticketsRow = index.row();
}

void MainWindow::on_refundsAdmin_clicked(const QModelIndex &index)
{
    refundsRow = index.row();
}

void MainWindow::on_deleteUsersAdmin_clicked()
{
    usersModel->removeRow(usersRow);
}

void MainWindow::on_deleteCitiesAdmin_clicked()
{
    citiesModel->removeRow(citiesRow);
}

void MainWindow::on_deleteRoutesAdmin_clicked()
{
    routesModel->removeRow(routesRow);
}

void MainWindow::on_deleteScheduleAdmin_clicked()
{
    scheduleModel->removeRow(scheduleRow);
}

void MainWindow::on_deleteTicketsAdmin_clicked()
{
    ticketsModel->removeRow(ticketsRow);
}

void MainWindow::on_deleteRefundsAdmin_clicked()
{
    refundsModel->removeRow(refundsRow);
}

void MainWindow::on_showRoutesButton_clicked()
{
    int from = ui->inputFrom->currentData().toInt();
    int to = ui->inputTo->currentData().toInt();
    QString date = ui->inputDate->date().toString("yyyy-MM-dd");
    QString sql =
        "SELECT schedule.id AS ID, "
        "cf.name AS FromCity, ct.name AS ToCity, "
        "departure_time AS Departure, arrival_time AS Arrival, "
        "price AS Price, seats_available AS Seats "
        "FROM schedule "
        "JOIN routes r ON schedule.route_id = r.id "
        "JOIN cities cf ON r.city_from_id = cf.id "
        "JOIN cities ct ON r.city_to_id = ct.id "
        "WHERE r.city_from_id = %1 AND r.city_to_id = %2 "
        "AND date(departure_time) = '%3';";
    scheduleQueryModel->setQuery(sql.arg(from).arg(to).arg(date), dbManager->getDatabase());
    if (scheduleQueryModel->lastError().isValid()) {
        showError("Ошибка: " + scheduleQueryModel->lastError().text());
    }
}

void MainWindow::on_bookButton_clicked()
{
    QModelIndex index = ui->bookTable->currentIndex();
    if (!index.isValid()) {
        showError("Выберите рейс!");
        return;
    }

    int scheduleId = scheduleQueryModel->record(index.row()).value("ID").toInt();
    int seats = scheduleQueryModel->record(index.row()).value("Seats").toInt();

    if (seats <= 0) {
        showError("Нет свободных мест!");
        return;
    }

    if (!currentUser->isValid()) {
        showError("Ошибка: пользователь не авторизован!");
        return;
    }

    if (!ticketManager->bookTicket(currentUser->getId(), scheduleId)) {
        showError("Ошибка при бронировании!");
        return;
    }

    QMessageBox::information(this, "Успех", "Билет успешно забронирован!");
    on_showRoutesButton_clicked();
    loadUserTickets();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
    {
        if (ui->usersAdmin->hasFocus() && usersRow >= 0) {
            usersModel->removeRow(usersRow);
            usersModel->submitAll();
            return;
        }

        if (ui->citiesAdmin->hasFocus() && citiesRow >= 0) {
            citiesModel->removeRow(citiesRow);
            citiesModel->submitAll();
            return;
        }

        if (ui->routesAdmin->hasFocus() && routesRow >= 0) {
            routesModel->removeRow(routesRow);
            routesModel->submitAll();
            return;
        }

        if (ui->scheduleAdmin->hasFocus() && scheduleRow >= 0) {
            scheduleModel->removeRow(scheduleRow);
            scheduleModel->submitAll();
            return;
        }

        if (ui->ticketsAdmin->hasFocus() && ticketsRow >= 0) {
            ticketsModel->removeRow(ticketsRow);
            ticketsModel->submitAll();
            return;
        }

        if (ui->refundsAdmin->hasFocus() && refundsRow >= 0) {
            refundsModel->removeRow(refundsRow);
            refundsModel->submitAll();
            return;
        }
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::on_cancelTicketButton_clicked()
{
    QModelIndex idx = ui->ticketsViewTable->currentIndex();
    if (!idx.isValid()) {
        showError("Выберите билет для возврата!");
        return;
    }

    int ticketId = myTicketsModel->record(idx.row()).value("ticket_id").toInt();
    int scheduleId = myTicketsModel->record(idx.row()).value("schedule_id").toInt();
    qDebug() << "Cancelling ticket:" << ticketId << "schedule:" << scheduleId;

    if (!ticketManager->cancelTicket(ticketId, scheduleId)) {
        showError("Ошибка при возврате билета!");
        return;
    }

    myTicketsModel->select();

    if (scheduleModel) {
        scheduleModel->select();
    }

    if (scheduleQueryModel->rowCount() > 0) {
        on_showRoutesButton_clicked();
    }

    QMessageBox::information(this, "Успех", "Билет успешно возвращен!");
    ui->ticketsViewTable->resizeColumnsToContents();
}

void MainWindow::loadUserTickets()
{
    myTicketsModel = new QSqlTableModel(this, dbManager->getDatabase());
    myTicketsModel->setTable("tickets_view");
    myTicketsModel->setFilter(QString("user_id = %1").arg(currentUser->getId()));
    myTicketsModel->select();
    ui->ticketsViewTable->setModel(myTicketsModel);
    ui->ticketsViewTable->hideColumn(0);
    ui->ticketsViewTable->hideColumn(8);
    ui->ticketsViewTable->hideColumn(9);
    ui->ticketsViewTable->resizeColumnsToContents();
}
