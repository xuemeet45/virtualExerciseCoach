#include "AuthManager.h"
#include "DatabaseManager.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <ctime>
#include <cstring>

AuthManager* global_auth_manager = nullptr; // Define the global instance here

AuthManager& AuthManager::getInstance() {
    static AuthManager instance;
    return instance;
}

bool AuthManager::authenticateUser(const Glib::ustring& username, const Glib::ustring& password) {
    if (!global_db_manager) return false;
    
    std::ostringstream oss;
    oss << "SELECT id, username, email, password_hash, first_name, last_name, created_at, last_login, is_active "
        << "FROM users WHERE username = '" << username << "' AND is_active = true";
    
    auto users = global_db_manager->fetch_users(oss.str());
    if (users.empty()) return false;
    
    const auto& user = users[0];
    if (!verifyPassword(password, user.get_password_hash())) return false;
    
    // Update last login
    std::ostringstream update_oss;
    update_oss << "UPDATE users SET last_login = CURRENT_TIMESTAMP WHERE id = " << user.get_id();
    global_db_manager->execute_query(update_oss.str());
    
    // Generate JWT token
    current_user = user;
    current_token = generateJWTToken(user);
    
    // Create session
    createSession(user.get_id(), current_token);
    
    return true;
}

bool AuthManager::registerUser(const Glib::ustring& username, const Glib::ustring& email, 
                              const Glib::ustring& password, const Glib::ustring& first_name, 
                              const Glib::ustring& last_name) {
    if (!global_db_manager) return false;
    
    // Check if user already exists
    std::ostringstream check_oss;
    check_oss << "SELECT id FROM users WHERE username = '" << username << "' OR email = '" << email << "'";
    auto existing_users = global_db_manager->fetch_users(check_oss.str());
    if (!existing_users.empty()) return false;
    
    // Hash password
    Glib::ustring hashed_password = hashPassword(password);
    
    // Insert new user
    std::ostringstream insert_oss;
    insert_oss << "INSERT INTO users (username, email, password_hash, first_name, last_name) "
               << "VALUES ('" << username << "', '" << email << "', '" << hashed_password << "', "
               << "'" << first_name << "', '" << last_name << "')";
    
    return global_db_manager->execute_query(insert_oss.str());
}

void AuthManager::logout() {
    if (!current_token.empty()) {
        invalidateSession(current_token);
    }
    current_user = User();
    current_token.clear();
}

bool AuthManager::change_password(int user_id, const std::string& old_password, const std::string& new_password) {
    if (!global_db_manager) return false;

    // Fetch user to verify old password
    std::ostringstream fetch_oss;
    fetch_oss << "SELECT id, username, email, password_hash, first_name, last_name, created_at, last_login, is_active "
              << "FROM users WHERE id = " << user_id;
    auto users = global_db_manager->fetch_users(fetch_oss.str());

    if (users.empty()) {
        std::cerr << "AuthManager: User not found for ID " << user_id << std::endl;
        return false;
    }

    const User& user_in_db = users[0];
    if (!verifyPassword(old_password, user_in_db.get_password_hash())) {
        std::cerr << "AuthManager: Old password verification failed for user ID " << user_id << std::endl;
        return false;
    }

    // Hash the new password
    Glib::ustring hashed_new_password = hashPassword(new_password);

    // Update password in the database
    std::ostringstream update_oss;
    update_oss << "UPDATE users SET password_hash = '" << hashed_new_password << "' WHERE id = " << user_id;

    return global_db_manager->execute_query(update_oss.str());
}

int AuthManager::get_current_user_id() const {
    return current_user.get_id();
}

Glib::ustring AuthManager::generateJWTToken(const User& user) {
    // Create JWT header
    std::string header = "{\"alg\":\"HS256\",\"typ\":\"JWT\"}";
    Glib::ustring header_b64 = base64Encode(header);
    
    // Create JWT payload
    std::time_t now = std::time(nullptr);
    std::time_t exp = now + (24 * 60 * 60); // 24 hours from now
    
    std::ostringstream payload_oss;
    payload_oss << "{\"user_id\":" << user.get_id() 
                << ",\"username\":\"" << user.get_username() 
                << "\",\"iat\":" << now 
                << ",\"exp\":" << exp << "}";
    
    Glib::ustring payload_b64 = base64Encode(payload_oss.str());
    
    // Create signature
    Glib::ustring data = header_b64 + "." + payload_b64;
    Glib::ustring signature = createHMAC(data, JWT_SECRET);
    Glib::ustring signature_b64 = base64Encode(signature);
    
    return data + "." + signature_b64;
}

bool AuthManager::validateJWTToken(const Glib::ustring& token) {
    if (token.empty()) return false;
    
    // Split token
    size_t first_dot = token.find('.');
    size_t second_dot = token.find('.', first_dot + 1);
    if (first_dot == Glib::ustring::npos || second_dot == Glib::ustring::npos) return false;
    
    Glib::ustring header_b64 = token.substr(0, first_dot);
    Glib::ustring payload_b64 = token.substr(first_dot + 1, second_dot - first_dot - 1);
    Glib::ustring signature_b64 = token.substr(second_dot + 1);
    
    // Verify signature
    Glib::ustring data = header_b64 + "." + payload_b64;
    Glib::ustring expected_signature = createHMAC(data, JWT_SECRET);
    Glib::ustring expected_signature_b64 = base64Encode(expected_signature);
    
    if (signature_b64 != expected_signature_b64) return false;
    
    // Check expiration
    std::string payload = base64Decode(payload_b64);
    // Simple JSON parsing for exp field
    size_t exp_pos = payload.find("\"exp\":");
    if (exp_pos != std::string::npos) {
        size_t exp_start = payload.find_first_not_of("0123456789", exp_pos + 6);
        if (exp_start != std::string::npos) {
            std::string exp_str = payload.substr(exp_pos + 6, exp_start - exp_pos - 6);
            std::time_t exp_time = std::stol(exp_str);
            if (std::time(nullptr) > exp_time) return false;
        }
    }
    
    return true;
}

User AuthManager::getUserFromToken(const Glib::ustring& token) {
    if (!validateJWTToken(token)) return User();
    
    size_t first_dot = token.find('.');
    size_t second_dot = token.find('.', first_dot + 1);
    Glib::ustring payload_b64 = token.substr(first_dot + 1, second_dot - first_dot - 1);
    
    std::string payload = base64Decode(payload_b64);
    
    // Extract user_id from payload
    size_t user_id_pos = payload.find("\"user_id\":");
    if (user_id_pos != std::string::npos) {
        size_t user_id_start = payload.find_first_not_of("0123456789", user_id_pos + 10);
        if (user_id_start != std::string::npos) {
            std::string user_id_str = payload.substr(user_id_pos + 10, user_id_start - user_id_pos - 10);
            int user_id = std::stoi(user_id_str);
            
            // Fetch user from database
            if (global_db_manager) {
                std::ostringstream oss;
                oss << "SELECT id, username, email, password_hash, first_name, last_name, created_at, last_login, is_active "
                    << "FROM users WHERE id = " << user_id;
                auto users = global_db_manager->fetch_users(oss.str());
                if (!users.empty()) return users[0];
            }
        }
    }
    
    return User();
}

bool AuthManager::createSession(int user_id, const Glib::ustring& token) {
    if (!global_db_manager) return false;
    
    std::ostringstream oss;
    oss << "INSERT INTO user_sessions (user_id, token_hash, expires_at) "
        << "VALUES (" << user_id << ", '" << token << "', "
        << "CURRENT_TIMESTAMP + INTERVAL '24 hours')";
    
    return global_db_manager->execute_query(oss.str());
}

bool AuthManager::invalidateSession(const Glib::ustring& token) {
    if (!global_db_manager) return false;
    
    std::ostringstream oss;
    oss << "UPDATE user_sessions SET is_valid = false WHERE token_hash = '" << token << "'";
    
    return global_db_manager->execute_query(oss.str());
}

bool AuthManager::isSessionValid(const Glib::ustring& token) {
    if (!global_db_manager) return false;
    
    std::ostringstream oss;
    oss << "SELECT id FROM user_sessions WHERE token_hash = '" << token 
        << "' AND is_valid = true AND expires_at > CURRENT_TIMESTAMP";
    
    auto sessions = global_db_manager->fetch_sessions(oss.str());
    return !sessions.empty();
}

Glib::ustring AuthManager::hashPassword(const Glib::ustring& password) {
    // Simple SHA-256 hashing (in production, use bcrypt or similar)
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.length());
    SHA256_Final(hash, &sha256);
    
    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    
    return oss.str();
}

bool AuthManager::verifyPassword(const Glib::ustring& password, const Glib::ustring& hash) {
    Glib::ustring computed_hash = hashPassword(password);
    return computed_hash == hash;
}

Glib::ustring AuthManager::base64Encode(const std::string& input) {
    const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    int val = 0, valb = -6;
    
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            result.push_back(chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    
    if (valb > -6) result.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (result.size() % 4) result.push_back('=');
    
    return result;
}

std::string AuthManager::base64Decode(const Glib::ustring& input) {
    const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    int val = 0, valb = -8;
    
    for (char c : input) {
        if (c == '=') break;
        size_t pos = chars.find(c);
        if (pos == std::string::npos) continue;
        
        val = (val << 6) + pos;
        valb += 6;
        if (valb >= 0) {
            result.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    
    return result;
}

Glib::ustring AuthManager::createHMAC(const Glib::ustring& data, const std::string& key) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;
    
    HMAC(EVP_sha256(), key.c_str(), key.length(),
         reinterpret_cast<const unsigned char*>(data.c_str()), data.length(),
         hash, &hash_len);
    
    std::ostringstream oss;
    for (unsigned int i = 0; i < hash_len; i++) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    
    return oss.str();
}

Glib::ustring AuthManager::generateRandomString(int length) {
    const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string result;
    result.reserve(length);
    
    for (int i = 0; i < length; ++i) {
        result += chars[rand() % chars.length()];
    }
    
    return result;
}
