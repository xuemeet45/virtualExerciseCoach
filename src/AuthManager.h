#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <string>
#include <glibmm/ustring.h>
#include "User.h"

class AuthManager {
public:
    static AuthManager& getInstance();
    
    // Authentication methods
    bool authenticateUser(const Glib::ustring& username, const Glib::ustring& password);
    bool registerUser(const Glib::ustring& username, const Glib::ustring& email, 
                     const Glib::ustring& password, const Glib::ustring& first_name, 
                     const Glib::ustring& last_name);
    void logout();
    bool change_password(int user_id, const std::string& old_password, const std::string& new_password);
    
    // JWT token methods
    Glib::ustring generateJWTToken(const User& user);
    bool validateJWTToken(const Glib::ustring& token);
    User getUserFromToken(const Glib::ustring& token);
    
    // Session management
    bool createSession(int user_id, const Glib::ustring& token);
    bool invalidateSession(const Glib::ustring& token);
    bool isSessionValid(const Glib::ustring& token);
    
    // Current user
    const User& getCurrentUser() const { return current_user; } // Return const reference
    bool isLoggedIn() const { return !current_token.empty(); }
    Glib::ustring getCurrentToken() const { return current_token; }
    int get_current_user_id() const;
    
    // Password hashing
    Glib::ustring hashPassword(const Glib::ustring& password);
    bool verifyPassword(const Glib::ustring& password, const Glib::ustring& hash);

private:
    AuthManager() = default;
    ~AuthManager() = default;
    AuthManager(const AuthManager&) = delete;
    AuthManager& operator=(const AuthManager&) = delete;
    
    User current_user;
    Glib::ustring current_token;
    
    // JWT secret key (in production, this should be stored securely)
    const std::string JWT_SECRET = "your-secret-key-change-in-production";
    
    // Helper methods
    Glib::ustring base64Encode(const std::string& input);
    std::string base64Decode(const Glib::ustring& input);
    Glib::ustring createHMAC(const Glib::ustring& data, const std::string& key);
    Glib::ustring generateRandomString(int length);
};

#endif // AUTHMANAGER_H
