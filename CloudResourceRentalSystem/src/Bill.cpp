#include "Bill.h"
#include "Utils.h" // For formatTimePoint
#include <iostream>
#include <iomanip> // For std::fixed, std::setprecision

// Constructor
Bill::Bill(std::string bId, std::string rId, std::string uId, double amt)
    : billId(bId), rentalId(rId), userId(uId), amount(amt),
      billDate(std::chrono::system_clock::now()), isPaid(false) {
}

// Getters
std::string Bill::getBillId() const {
    return billId;
}

std::string Bill::getRentalId() const {
    return rentalId;
}

std::string Bill::getUserId() const {
    return userId;
}

double Bill::getAmount() const {
    return amount;
}

std::chrono::system_clock::time_point Bill::getBillDate() const {
    return billDate;
}

bool Bill::getIsPaid() const {
    return isPaid;
}

// Setters
void Bill::setPaid(bool status) {
    this->isPaid = status;
}

// Display and helper functions
void Bill::displayBillInfo() const {
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Bill ID: " << billId << std::endl;
    std::cout << "Rental ID: " << rentalId << std::endl;
    std::cout << "User ID: " << userId << std::endl;
    std::cout << "Amount: $" << std::fixed << std::setprecision(2) << amount << std::endl;
    std::cout << "Bill Date: " << formatTimePoint(billDate) << std::endl;
    std::cout << "Status: " << (isPaid ? "Paid" : "Unpaid") << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}
