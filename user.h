#ifndef USER_H
#define USER_H

#include <QString>

class User {
public:
    User();
    User(int id, const QString &phone, const QString &role);

    int getId() const;
    QString getPhone() const;
    QString getRole() const;

    void setId(int id);
    void setPhone(const QString &phone);
    void setRole(const QString &role);

    bool isValid() const;
    bool isAdmin() const;

private:
    int id;
    QString phone;
    QString role;
};

#endif // USER_H
