#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include <vector>
#include <map>
#include <variant> // Included as per instruction, though specs map is used for now

// Define enums for ResourceType and ResourceStatus
enum class ResourceType {
    CPU,
    GPU,
    STORAGE
};

enum class ResourceStatus {
    IDLE,
    IN_USE
};

class Resource {
private:
    std::string resourceId;
    ResourceType type;
    std::string name;
    std::map<std::string, std::string> specs;
    ResourceStatus status;
    double pricePerHour;

public:
    // Constructor
    Resource(std::string id, ResourceType rType, std::string rName, 
             std::map<std::string, std::string> rSpecs, double rPricePerHour);

    // Getters
    std::string getResourceId() const;
    ResourceType getType() const;
    std::string getName() const;
    std::string getSpec(const std::string& key) const; // Get a specific spec
    std::map<std::string, std::string> getAllSpecs() const; // Get all specs
    ResourceStatus getStatus() const;
    double getPricePerHour() const;

    // Setters
    void setStatus(ResourceStatus newStatus);
    void setPricePerHour(double newPrice); // For admin use
    void setName(const std::string& newName); // For admin use
    void setSpecs(const std::map<std::string, std::string>& newSpecs); // For admin use

    // Display and helper functions
    void displayResourceInfo() const;
    std::string resourceTypeToString() const; // Helper to convert enum to string for display
};

#endif // RESOURCE_H
