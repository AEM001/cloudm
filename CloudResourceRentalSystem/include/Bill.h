#ifndef BILL_H
#define BILL_H

#include <string>
#include <chrono>
#include <vector> // Though not directly used in Bill members, good for consistency

class Bill {
private:
    std::string billId;
    std::string rentalId;
    std::string userId;
    double amount;
    std::chrono::system_clock::time_point billDate;
    bool isPaid; // Status of the bill

public:
    // Constructor
    Bill(std::string bId, std::string rId, std::string uId, double amt);

    // Getters
    std::string getBillId() const;
    std::string getRentalId() const;
    std::string getUserId() const;
    double getAmount() const;
    std::chrono::system_clock::time_point getBillDate() const;
    bool getIsPaid() const;

    // Setters
    void setPaid(bool status);

    // Display and helper functions
    void displayBillInfo() const;
};

#endif // BILL_H
