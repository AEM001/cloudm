#ifndef RENTAL_H
#define RENTAL_H

#include <string>
#include <chrono>
#include "User.h"       // For userId association
#include "Resource.h"   // For resourceId association

// Define enum for RentalStatus
enum class RentalStatus {
    PENDING_APPROVAL,
    APPROVED,
    REJECTED,
    ACTIVE,
    COMPLETED,
    CANCELLED
};

class Rental {
private:
    std::string rentalId;
    std::string userId;
    std::string resourceId;
    std::chrono::system_clock::time_point startTime; // Requested or actual start time
    std::chrono::system_clock::time_point endTime;   // Requested or actual end time
    std::chrono::system_clock::time_point requestTime; // Time the rental request was made
    RentalStatus status;
    double totalCost; // Calculated when rental is completed or on approval

public:
    // Constructor
    Rental(std::string id, std::string uId, std::string rId, 
           std::chrono::system_clock::time_point sTime, 
           std::chrono::system_clock::time_point eTime);

    // Getters
    std::string getRentalId() const;
    std::string getUserId() const;
    std::string getResourceId() const;
    std::chrono::system_clock::time_point getStartTime() const;
    std::chrono::system_clock::time_point getEndTime() const;
    std::chrono::system_clock::time_point getRequestTime() const;
    RentalStatus getStatus() const;
    double getTotalCost() const;

    // Setters
    void setStatus(RentalStatus newStatus);
    void setTotalCost(double cost);
    void setStartTime(std::chrono::system_clock::time_point sTime); // For admin approval/adjustment
    void setEndTime(std::chrono::system_clock::time_point eTime);   // For admin approval/adjustment

    // Helper and display functions
    std::string rentalStatusToString() const; // Helper to convert enum to string
    void displayRentalInfo() const; // Prints rental details (will use Utils::formatTimePoint)
};

#endif // RENTAL_H
