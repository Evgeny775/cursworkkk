#include "user.h"

User::User() : id(-1), phone(""), role("") {}

User::User(int id, const QString &phone, const QString &role)
    : id(id), phone(phone), role(role) {}

int User::getId() const {
    return id;
}

QString User::getPhone() const {
    return phone;
}

QString User::getRole() const {
    return role;
}

void User::setId(int id) {
    this->id = id;
}

void User::setPhone(const QString &phone) {
    this->phone = phone;
}

void User::setRole(const QString &role) {
    this->role = role;
}

bool User::isValid() const {
    return id > 0;
}

bool User::isAdmin() const {
    return role == "admin";
}
