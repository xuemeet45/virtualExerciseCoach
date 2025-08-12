#ifndef USER_H
#define USER_H

#include <string>
#include <glibmm/ustring.h>

class User {
public:
    User(int id = 0, const Glib::ustring& username = "",
         const Glib::ustring& email = "",
         const Glib::ustring& password_hash = "",
         const Glib::ustring& first_name = "",
         const Glib::ustring& last_name = "",
         const Glib::ustring& created_at = "",
         const Glib::ustring& last_login = "",
         bool is_active = true)
        : id(id), username(username), email(email), password_hash(password_hash), first_name(first_name),
          last_name(last_name), created_at(created_at), last_login(last_login),
          is_active(is_active) {}

    int get_id() const { return id; }
    Glib::ustring get_username() const { return username; }
    Glib::ustring get_email() const { return email; }
    Glib::ustring get_password_hash() const { return password_hash; }
    Glib::ustring get_first_name() const { return first_name; }
    Glib::ustring get_last_name() const { return last_name; }
    Glib::ustring get_created_at() const { return created_at; }
    Glib::ustring get_last_login() const { return last_login; }
    bool get_is_active() const { return is_active; }

    void set_last_login(const Glib::ustring& login_time) { last_login = login_time; }

private:
    int id;
    Glib::ustring username;
    Glib::ustring email;
    Glib::ustring password_hash;
    Glib::ustring first_name;
    Glib::ustring last_name;
    Glib::ustring created_at;
    Glib::ustring last_login;
    bool is_active;
};

#endif // USER_H 