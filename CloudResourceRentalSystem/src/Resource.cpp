#include "Resource.h"
#include <iostream>
#include <iomanip> // For std::fixed and std::setprecision

// Constructor
Resource::Resource(std::string id, ResourceType rType, std::string rName, 
                   std::map<std::string, std::string> rSpecs, double rPricePerHour)
    : resourceId(id), type(rType), name(rName), specs(rSpecs), 
      pricePerHour(rPricePerHour), status(ResourceStatus::IDLE) {
    // Status is initialized to IDLE by default
}

// Getters
std::string Resource::getResourceId() const {
    return resourceId;
}

ResourceType Resource::getType() const {
    return type;
}

std::string Resource::getName() const {
    return name;
}

std::string Resource::getSpec(const std::string& key) const {
    auto it = specs.find(key);
    if (it != specs.end()) {
        return it->second;
    }
    return "N/A"; // Or throw an exception, or return an optional
}

std::map<std::string, std::string> Resource::getAllSpecs() const {
    return specs;
}

ResourceStatus Resource::getStatus() const {
    return status;
}

double Resource::getPricePerHour() const {
    return pricePerHour;
}

// Setters
void Resource::setStatus(ResourceStatus newStatus) {
    this->status = newStatus;
}

void Resource::setPricePerHour(double newPrice) {
    this->pricePerHour = newPrice; // Typically only for admins
}

void Resource::setName(const std::string& newName) {
    this->name = newName; // Typically only for admins
}

void Resource::setSpecs(const std::map<std::string, std::string>& newSpecs) {
    this->specs = newSpecs; // Typically only for admins
}

// Helper function to convert ResourceType enum to string
std::string Resource::resourceTypeToString() const {
    switch (type) {
        case ResourceType::CPU:     return "CPU";
        case ResourceType::GPU:     return "GPU";
        case ResourceType::STORAGE: return "Storage";
        default:                    return "Unknown Type";
    }
}

// Display resource information
void Resource::displayResourceInfo() const {
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Resource ID: " << resourceId << std::endl;
    std::cout << "Name: " << name << std::endl;
    std::cout << "Type: " << resourceTypeToString() << std::endl;
    std::cout << "Status: ";
    switch (status) {
        case ResourceStatus::IDLE:   std::cout << "Idle";   break;
        case ResourceStatus::IN_USE: std::cout << "In Use"; break;
        default:                     std::cout << "Unknown";break;
    }
    std::cout << std::endl;
    std::cout << "Price per hour: $" << std::fixed << std::setprecision(2) << pricePerHour << std::endl;
    
    if (!specs.empty()) {
        std::cout << "Specifications:" << std::endl;
        for (const auto& spec : specs) {
            std::cout << "  " << spec.first << ": " << spec.second << std::endl;
        }
    }
    std::cout << "----------------------------------------" << std::endl;
}
