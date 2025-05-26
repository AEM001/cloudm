#include "User.h"
#include <iostream>
#include <iomanip> // For std::fixed and std::setprecision

// Helper function for password hashing (simple XOR hash)
// This function is defined in the header for inline usage or can be made static in .cpp
// For this implementation, I'll keep its definition within the .cpp and make it a static helper
// or a free function if it doesn't need class member access.
// Since it's already in the header as a private member, I'll implement it as part of the class.
std::string User::xorHash(const std::string& input) const {
    std::string hashed = input;
    char key = 'K'; // Simple key, should be more complex or managed securely
    for (char &c : hashed) {
        c ^= key;
    }
    return hashed;
}

// Constructor
User::User(std::string id, std::string uname, std::string passwd, UserRole r, std::string realName)
    : userId(id), username(uname), role(r), name(realName), balance(0.0), status(UserStatus::ACTIVE) {
    this->passwordHash = xorHash(passwd);
}

// Getters
std::string User::getUserId() const {
    return userId;
}

std::string User::getUsername() const {
    return username;
}

UserRole User::getRole() const {
    return role;
}

double User::getBalance() const {
    return balance;
}

UserStatus User::getStatus() const {
    return status;
}

std::string User::getName() const {
    return name;
}

// Setters
void User::setPassword(const std::string& newPassword) {
    this->passwordHash = xorHash(newPassword);
}

void User::setBalance(double newBalance) {
    this->balance = newBalance;
}

void User::setStatus(UserStatus newStatus) {
    this->status = newStatus;
}

void User::setName(const std::string& newName) {
    this->name = newName;
}

void User::setRole(UserRole newRole) {
    this->role = newRole;
}

// Password checking
bool User::checkPassword(const std::string& passwd) const {
    return xorHash(passwd) == this->passwordHash;
}

// Display user information
void User::displayUserInfo() const {
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "User ID: " << userId << std::endl;
    std::cout << "Username: " << username << std::endl;
    std::cout << "Name: " << name << std::endl;
    std::cout << "Role: ";
    switch (role) {
        case UserRole::STUDENT: std::cout << "Student"; break;
        case UserRole::TEACHER: std::cout << "Teacher"; break;
        case UserRole::ADMIN:   std::cout << "Admin";   break;
        default:                std::cout << "Unknown"; break;
    }
    std::cout << std::endl;
    std::cout << "Status: ";
    switch (status) {
        case UserStatus::ACTIVE:    std::cout << "Active";    break;
        case UserStatus::SUSPENDED: std::cout << "Suspended"; break;
        default:                    std::cout << "Unknown";   break;
    }
    std::cout << std::endl;
    std::cout << "Balance: $" << std::fixed << std::setprecision(2) << balance << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}
