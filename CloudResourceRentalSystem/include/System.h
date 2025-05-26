#ifndef SYSTEM_H
#define SYSTEM_H

#include "User.h"
#include "Utils.h" // For generateUniqueId
#include "Resource.h" // Added for Resource management
#include "Rental.h"   // Added for Rental management
#include "Bill.h"     // Added for Bill management
#include <vector>
#include <string>
#include <memory> // For std::unique_ptr or std::shared_ptr

class System {
private:
    std::vector<User> users;
    User* currentUser; // Raw pointer to the currently logged-in user
    std::vector<Resource> resources; // Container for resources
    std::vector<Rental> rentals;     // Container for rentals
    std::vector<Bill> bills;         // Container for bills

    // Private helper methods
    User* findUser(const std::string& username); // Finds by username
    User* findUserById(const std::string& userId); // Finds by userId
    // findResource is public as per requirement

public:
    // Constructor
    System();

    // Destructor
    ~System();

    // User management functions
    bool registerUser(const std::string& username, const std::string& password, UserRole role, const std::string& realName);
    User* loginUser(const std::string& username, const std::string& password);
    void logoutUser();
    User* getCurrentUser() const;
    void displayAllUsers() const; // Changed from optional to standard

    // Personal information management for the current user
    bool updateCurrentUserName(const std::string& newName);
    bool updateCurrentUserPassword(const std::string& newPassword);

    // Resource management functions
    bool addResource(const Resource& resource);
    Resource* findResource(const std::string& resourceId); // Made public
    void displayAllResources() const;
    std::vector<Resource*> findResourcesByType(ResourceType type); // Non-const because it returns non-const pointers

    // Rental management functions
    bool requestResourceRental(const std::string& resourceId, int durationHours);
    std::vector<Rental*> getUserRentals(const std::string& userId); // Returns non-const pointers
    Rental* findRental(const std::string& rentalId); // Returns non-const pointer
    void displayUserRentals(const std::string& userId); // Should be const if only displaying

    // Rental cancellation
    bool cancelRentalRequest(const std::string& rentalId);

    // Admin Resource Management
    bool adminModifyResource(const std::string& resourceId, const std::string& newName, 
                             const std::map<std::string, std::string>& newSpecs, double newPricePerHour);
    bool adminDeleteResource(const std::string& resourceId);

    // Admin User Management
    void adminDisplayAllUsers() const;
    bool adminAddUser(const std::string& username, const std::string& password, UserRole role, const std::string& realName);
    bool adminModifyUser(const std::string& targetUsername, const std::string& newRealName, 
                         UserRole newRole, UserStatus newStatus, double newBalance);
    bool adminSetUserStatus(const std::string& targetUsername, UserStatus newStatus);

    // Admin Rental Review
    void adminDisplayPendingRentals() const;
    bool adminApproveRental(const std::string& rentalId);
    bool adminRejectRental(const std::string& rentalId, const std::string& reason);

    // Admin View All Rentals
    void adminDisplayAllRentals() const;

    // Billing
    bool processRentalCompletion(const std::string& rentalId);
    void displayUserBills(const std::string& userId) const;
    void adminDisplayAllBills() const;
};

#endif // SYSTEM_H
