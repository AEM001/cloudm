#ifndef USER_H
#define USER_H

#include <string>
#include <vector> // Included as per instruction, though not used yet

// Define enums for UserRole and UserStatus
enum class UserRole {
    STUDENT,
    TEACHER,
    ADMIN
};

enum class UserStatus {
    ACTIVE,
    SUSPENDED
};

class User {
private:
    std::string userId;
    std::string username;
    std::string passwordHash; // Store a hash of the password, not plaintext
    UserRole role;
    double balance;
    UserStatus status;
    std::string name; // Real name

    // Helper function for password hashing (simple XOR hash)
    std::string xorHash(const std::string& input) const;

public:
    // Constructor
    User(std::string id, std::string uname, std::string passwd, UserRole r, std::string realName);

    // Getters
    std::string getUserId() const;
    std::string getUsername() const;
    UserRole getRole() const;
    double getBalance() const;
    UserStatus getStatus() const;
    std::string getName() const;

    // Setters
    void setPassword(const std::string& newPassword);
    void setBalance(double newBalance);
    void setStatus(UserStatus newStatus);
    void setName(const std::string& newName);
    void setRole(UserRole newRole); // Added setter for role

    // Password checking
    bool checkPassword(const std::string& passwd) const;

    // Display user information
    void displayUserInfo() const;
};

#endif // USER_H
