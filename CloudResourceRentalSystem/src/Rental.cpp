#include "Rental.h"
#include "Utils.h" // For formatTimePoint
#include <iostream>
#include <iomanip> // For std::fixed, std::setprecision
#include <ctime>   // For std::localtime, std::put_time (used in Utils::formatTimePoint)

// Constructor
Rental::Rental(std::string id, std::string uId, std::string rId,
               std::chrono::system_clock::time_point sTime,
               std::chrono::system_clock::time_point eTime)
    : rentalId(id), userId(uId), resourceId(rId), startTime(sTime), endTime(eTime),
      requestTime(std::chrono::system_clock::now()),
      status(RentalStatus::PENDING_APPROVAL), totalCost(0.0) {
}

// Getters
std::string Rental::getRentalId() const {
    return rentalId;
}

std::string Rental::getUserId() const {
    return userId;
}

std::string Rental::getResourceId() const {
    return resourceId;
}

std::chrono::system_clock::time_point Rental::getStartTime() const {
    return startTime;
}

std::chrono::system_clock::time_point Rental::getEndTime() const {
    return endTime;
}

std::chrono::system_clock::time_point Rental::getRequestTime() const {
    return requestTime;
}

RentalStatus Rental::getStatus() const {
    return status;
}

double Rental::getTotalCost() const {
    return totalCost;
}

// Setters
void Rental::setStatus(RentalStatus newStatus) {
    this->status = newStatus;
}

void Rental::setTotalCost(double cost) {
    this->totalCost = cost;
}

void Rental::setStartTime(std::chrono::system_clock::time_point sTime) {
    this->startTime = sTime;
}

void Rental::setEndTime(std::chrono::system_clock::time_point eTime) {
    this->endTime = eTime;
}

// Helper to convert RentalStatus enum to string
std::string Rental::rentalStatusToString() const {
    switch (status) {
        case RentalStatus::PENDING_APPROVAL: return "Pending Approval";
        case RentalStatus::APPROVED:         return "Approved";
        case RentalStatus::REJECTED:         return "Rejected";
        case RentalStatus::ACTIVE:           return "Active";
        case RentalStatus::COMPLETED:        return "Completed";
        case RentalStatus::CANCELLED:        return "Cancelled";
        default:                             return "Unknown Status";
    }
}

// Display rental information
void Rental::displayRentalInfo() const {
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Rental ID: " << rentalId << std::endl;
    std::cout << "User ID: " << userId << std::endl;
    std::cout << "Resource ID: " << resourceId << std::endl;
    std::cout << "Status: " << rentalStatusToString() << std::endl;
    std::cout << "Request Time: " << formatTimePoint(requestTime) << std::endl;
    std::cout << "Start Time: " << formatTimePoint(startTime) << std::endl;
    std::cout << "End Time: " << formatTimePoint(endTime) << std::endl;
    std::cout << "Total Cost: $" << std::fixed << std::setprecision(2) << totalCost << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}
