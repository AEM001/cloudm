#include "System.h"
#include "User.h" // Included for User class definition, though System.h includes it
#include "Utils.h"  // For generateUniqueId
#include <iostream>
#include <algorithm> // For std::find_if
#include <sstream>   // Not strictly needed here if using Utils::generateUniqueId
#include <iomanip>   // For std::fixed and std::setprecision

// Constructor
System::System() : currentUser(nullptr) {
    // Initialization, if any, can go here
}

// Destructor
System::~System() {
    // Cleanup, if any (e.g., if using raw pointers for users and dynamically allocating them)
    // If users vector holds objects directly, or smart pointers, this might not be needed.
}

// Private helper method to find a user by username
User* System::findUser(const std::string& username) {
    auto it = std::find_if(users.begin(), users.end(), 
                           [&username](const User& u) { return u.getUsername() == username; });
    if (it != users.end()) {
        return &(*it); // Return a pointer to the found user
    }
    return nullptr; // User not found
}

// Private helper method to find a user by user ID
User* System::findUserById(const std::string& userId) {
    auto it = std::find_if(users.begin(), users.end(),
                           [&userId](const User& u) { return u.getUserId() == userId; });
    if (it != users.end()) {
        return &(*it); // Return a pointer to the found user
    }
    return nullptr; // User not found
}

// User management functions
bool System::registerUser(const std::string& username, const std::string& password, UserRole role, const std::string& realName) {
    if (findUser(username)) {
        std::cout << "Error: Username '" << username << "' already exists." << std::endl;
        return false; // Username already exists
    }

    // Generate a unique ID for the user
    std::string userId = generateUniqueId("user_", users.size());

    // Create and add the new user
    users.emplace_back(userId, username, password, role, realName);
    std::cout << "User '" << username << "' registered successfully with ID: " << userId << std::endl;
    return true;
}

// Admin View All Rentals
void System::adminDisplayAllRentals() const {
    if (!currentUser || currentUser->getRole() != UserRole::ADMIN) {
        std::cout << "Error: Admin privileges required to display all rental records." << std::endl;
        return;
    }

    std::cout << "\n--- All Rental Records (Admin View) ---" << std::endl;
    if (rentals.empty()) {
        std::cout << "No rental records found in the system." << std::endl;
    } else {
        for (const auto& rental : rentals) {
            rental.displayRentalInfo();
        }
    }
    std::cout << "---------------------------------------" << std::endl;
}

// Billing
bool System::processRentalCompletion(const std::string& rentalId) {
    Rental* rental = findRental(rentalId);
    if (!rental) {
        std::cout << "Error: Rental with ID '" << rentalId << "' not found for processing completion." << std::endl;
        return false;
    }

    if (rental->getStatus() != RentalStatus::APPROVED && rental->getStatus() != RentalStatus::ACTIVE) {
        std::cout << "Error: Rental '" << rentalId << "' is not in APPROVED or ACTIVE state. Current status: "
                  << rental->rentalStatusToString() << ". Cannot process completion." << std::endl;
        return false;
    }

    // For simplicity, we assume endTime has been reached. A real system would check this.

    Resource* resource = findResource(rental->getResourceId());
    if (!resource) {
        std::cout << "Error: Associated resource with ID '" << rental->getResourceId() 
                  << "' for rental '" << rentalId << "' not found. Cannot process completion." << std::endl;
        // Potentially mark rental as problematic, e.g., needs investigation
        return false;
    }

    User* user = findUserById(rental->getUserId()); // Corrected: Use findUserById
    if (!user) {
        std::cout << "Error: Associated user with ID '" << rental->getUserId() 
                  << "' for rental '" << rentalId << "' not found. Cannot process completion." << std::endl;
        return false;
    }

    // Calculate duration and cost
    auto duration = std::chrono::duration_cast<std::chrono::hours>(rental->getEndTime() - rental->getStartTime());
    long long durationHours = duration.count();
    if (durationHours == 0) { // Minimum 1 hour rule
        // This rule applies if the duration is less than 1 full hour.
        // E.g. start=10:00, end=10:30 -> duration.count() is 0.
        // If start=10:00, end=11:30 -> duration.count() is 1.
        // A more precise calculation might be ceil(duration_in_minutes / 60.0)
        // For now, if chrono::hours gives 0, we set it to 1.
        // If rental was for, say, 30 minutes, std::chrono::hours will truncate to 0.
        // If it was 1 hour 30 minutes, it will truncate to 1.
        // The prompt's minimum 1 hour rule for "durationHours == 0" is interpreted as:
        // if the truncated hour count is 0 (i.e., duration < 1 hour), bill for 1 hour.
        // If duration is >= 1 hour, use the truncated hour count.
        // For example, 0.5 hours -> 1 hour bill. 1.5 hours -> 1 hour bill.
        // This interpretation matches "if (durationHours == 0) durationHours = 1;".
        // If the intent was "always round up to the nearest hour", the calculation would be different.
        // E.g. using std::ceil on a double representation of hours.
        // Given the constraints, the simple `if (durationHours == 0) durationHours = 1;` is used.
        durationHours = 1; 
    }
    
    double cost = durationHours * resource->getPricePerHour();

    rental->setTotalCost(cost);
    rental->setStatus(RentalStatus::COMPLETED);
    resource->setStatus(ResourceStatus::IDLE); // Resource becomes available

    std::string billId = generateUniqueId("bill_", bills.size());
    Bill newBill(billId, rentalId, user->getUserId(), cost);
    
    // Deduct from user balance and mark bill as paid
    double currentBalance = user->getBalance();
    user->setBalance(currentBalance - cost);
    newBill.setPaid(true); // Direct deduction model

    bills.push_back(newBill);

    std::cout << "Rental '" << rentalId << "' completed. Bill '" << billId << "' generated for $" << cost 
              << ". User '" << user->getUsername() << "' balance updated to $" << user->getBalance() << "." << std::endl;

    if (user->getBalance() < 0) {
        std::cout << "Warning: User '" << user->getUsername() << "' balance is negative: $" 
                  << std::fixed << std::setprecision(2) << user->getBalance() << "." << std::endl;
        // Future: Trigger notification
    }
    return true;
}

void System::displayUserBills(const std::string& userId) const {
    // Permission checks:
    // 1. A user must be logged in.
    // 2. The logged-in user must either be the user whose bills are requested OR an admin.
    if (!currentUser) {
         std::cout << "Error: No user logged in. Cannot display bills." << std::endl;
         return;
    }

    if (currentUser->getUserId() != userId && currentUser->getRole() != UserRole::ADMIN) {
        std::cout << "Error: You do not have permission to view bills for User ID '" << userId << "'." << std::endl;
        return;
    }
    
    std::cout << "\n--- Bills for User ID: " << userId << " ---" << std::endl;
    bool found = false;
    for (const auto& bill : bills) {
        if (bill.getUserId() == userId) {
            bill.displayBillInfo();
            found = true;
        }
    }
    if (!found) {
        std::cout << "No bills found for User ID '" << userId << "'." << std::endl;
    }
    std::cout << "----------------------------------" << std::endl;
}

void System::adminDisplayAllBills() const {
    if (!currentUser || currentUser->getRole() != UserRole::ADMIN) {
        std::cout << "Error: Admin privileges required to display all bills." << std::endl;
        return;
    }

    std::cout << "\n--- All Bills (Admin View) ---" << std::endl;
    if (bills.empty()) {
        std::cout << "No bills found in the system." << std::endl;
    } else {
        for (const auto& bill : bills) {
            bill.displayBillInfo();
        }
    }
    std::cout << "------------------------------" << std::endl;
}

// Admin Rental Review
void System::adminDisplayPendingRentals() const {
    if (!currentUser || currentUser->getRole() != UserRole::ADMIN) {
        std::cout << "Error: Admin privileges required to display pending rentals." << std::endl;
        return;
    }

    std::cout << "\n--- Pending Rental Requests (Admin View) ---" << std::endl;
    bool found = false;
    for (const auto& rental : rentals) {
        if (rental.getStatus() == RentalStatus::PENDING_APPROVAL) {
            rental.displayRentalInfo();
            found = true;
        }
    }
    if (!found) {
        std::cout << "No rental requests currently pending approval." << std::endl;
    }
    std::cout << "--------------------------------------------" << std::endl;
}

bool System::adminApproveRental(const std::string& rentalId) {
    if (!currentUser || currentUser->getRole() != UserRole::ADMIN) {
        std::cout << "Error: Admin privileges required to approve rentals." << std::endl;
        return false;
    }

    Rental* rentalToApprove = findRental(rentalId);
    if (!rentalToApprove) {
        std::cout << "Error: Rental with ID '" << rentalId << "' not found." << std::endl;
        return false;
    }

    if (rentalToApprove->getStatus() != RentalStatus::PENDING_APPROVAL) {
        std::cout << "Error: Rental '" << rentalId << "' is not pending approval. Current status: " 
                  << rentalToApprove->rentalStatusToString() << "." << std::endl;
        return false;
    }

    Resource* resourceToUse = findResource(rentalToApprove->getResourceId());
    if (!resourceToUse) {
        std::cout << "Error: Associated resource with ID '" << rentalToApprove->getResourceId() 
                  << "' for rental '" << rentalId << "' not found. Cannot approve." << std::endl;
        // Optionally, set rental to REJECTED here if resource is permanently gone
        // rentalToApprove->setStatus(RentalStatus::REJECTED);
        return false;
    }

    if (resourceToUse->getStatus() != ResourceStatus::IDLE) {
        std::string statusStr = (resourceToUse->getStatus() == ResourceStatus::IDLE) ? "Idle" : "In Use";
        std::cout << "Error: Resource '" << resourceToUse->getName() << "' (ID: " << resourceToUse->getResourceId() 
                  << ") is currently not IDLE. Status: " << statusStr << ". Cannot approve rental '" << rentalId << "'." << std::endl;
        return false;
    }

    rentalToApprove->setStatus(RentalStatus::APPROVED); 
    // Or RentalStatus::ACTIVE if startTime is now/past. For simplicity, using APPROVED.
    // Actual activation could be a separate timed process or when user explicitly starts it.
    resourceToUse->setStatus(ResourceStatus::IN_USE);

    std::cout << "Rental '" << rentalId << "' approved by admin '" << currentUser->getUsername() 
              << "'. Resource '" << resourceToUse->getName() << "' is now IN_USE." << std::endl;
    // Placeholder for notification to user
    return true;
}

bool System::adminRejectRental(const std::string& rentalId, const std::string& reason) {
    if (!currentUser || currentUser->getRole() != UserRole::ADMIN) {
        std::cout << "Error: Admin privileges required to reject rentals." << std::endl;
        return false;
    }

    Rental* rentalToReject = findRental(rentalId);
    if (!rentalToReject) {
        std::cout << "Error: Rental with ID '" << rentalId << "' not found." << std::endl;
        return false;
    }

    if (rentalToReject->getStatus() != RentalStatus::PENDING_APPROVAL) {
        std::cout << "Error: Rental '" << rentalId << "' is not pending approval. Current status: " 
                  << rentalToReject->rentalStatusToString() << "." << std::endl;
        return false;
    }

    rentalToReject->setStatus(RentalStatus::REJECTED);
    // If Rental class had a rejectionReason field, set it here:
    // rentalToReject->setRejectionReason(reason); 

    std::cout << "Rental '" << rentalId << "' rejected by admin '" << currentUser->getUsername() 
              << "'. Reason: " << reason << "." << std::endl;
    // Placeholder for notification to user
    return true;
}

User* System::loginUser(const std::string& username, const std::string& password) {
    User* userToLogin = findUser(username);
    if (userToLogin) {
        if (userToLogin->checkPassword(password)) {
            if (userToLogin->getStatus() == UserStatus::ACTIVE) {
                currentUser = userToLogin;
                std::cout << "User '" << username << "' logged in successfully." << std::endl;
                return currentUser;
            } else {
                std::cout << "Login failed: User '" << username << "' account is suspended." << std::endl;
                return nullptr;
            }
        } else {
            std::cout << "Login failed: Incorrect password for user '" << username << "'." << std::endl;
        }
    } else {
        std::cout << "Login failed: User '" << username << "' not found." << std::endl;
    }
    return nullptr; // Login failed
}

void System::logoutUser() {
    if (currentUser) {
        std::cout << "User '" << currentUser->getUsername() << "' logged out." << std::endl;
        currentUser = nullptr;
    } else {
        std::cout << "No user currently logged in." << std::endl;
    }
}

User* System::getCurrentUser() const {
    return currentUser;
}

// (Optional) Method to display all users - for debugging or admin purposes
void System::displayAllUsers() const {
    if (users.empty()) {
        std::cout << "No users registered in the system." << std::endl;
        return;
    }
    std::cout << "\n--- All Registered Users ---" << std::endl;
    for (const auto& user : users) {
        user.displayUserInfo();
    }
    std::cout << "----------------------------" << std::endl;
}

// Personal information management for the current user
bool System::updateCurrentUserName(const std::string& newName) {
    if (currentUser) {
        currentUser->setName(newName);
        return true;
    }
    std::cout << "Error: No user is currently logged in. Cannot update name." << std::endl;
    return false;
}

bool System::updateCurrentUserPassword(const std::string& newPassword) {
    if (currentUser) {
        currentUser->setPassword(newPassword);
        return true;
    }
    std::cout << "Error: No user is currently logged in. Cannot update password." << std::endl;
    return false;
}

// Resource management functions
bool System::addResource(const Resource& resource) {
    // Check for duplicate resource ID
    if (findResource(resource.getResourceId())) {
        std::cout << "Error: Resource with ID '" << resource.getResourceId() << "' already exists." << std::endl;
        return false;
    }
    resources.push_back(resource);
    std::cout << "Resource '" << resource.getName() << "' added successfully with ID: " << resource.getResourceId() << std::endl;
    return true;
}

Resource* System::findResource(const std::string& resourceId) {
    auto it = std::find_if(resources.begin(), resources.end(),
                           [&resourceId](const Resource& r) { return r.getResourceId() == resourceId; });
    if (it != resources.end()) {
        return &(*it); // Return a pointer to the found resource
    }
    return nullptr; // Resource not found
}

void System::displayAllResources() const {
    if (resources.empty()) {
        std::cout << "No resources available in the system." << std::endl;
        return;
    }
    std::cout << "\n--- All Available Resources ---" << std::endl;
    for (const auto& resource : resources) {
        resource.displayResourceInfo();
    }
    std::cout << "-------------------------------" << std::endl;
}

// Note: The subtask description had findResourcesByType as const, 
// but returning a vector of non-const pointers.
// If the intent is to modify resources through these pointers, the method itself cannot be const.
// Or, it should return std::vector<const Resource*>
// For now, I'll match the signature which implies modification is possible, so method is not const.
// If Resource objects themselves should not be modified via this function, 
// then vector<const Resource*> is better and the method can be const.
// I'll proceed with vector<Resource*> and non-const method as per the prompt's signature.
std::vector<Resource*> System::findResourcesByType(ResourceType type) {
    std::vector<Resource*> foundResources;
    for (auto& resource : resources) { // Use auto& to allow taking address of non-const
        if (resource.getType() == type) {
            foundResources.push_back(&resource);
        }
    }
    return foundResources;
}

// Rental management functions
bool System::requestResourceRental(const std::string& resourceId, int durationHours) {
    if (!currentUser) {
        std::cout << "Error: No user logged in. Please log in to request a rental." << std::endl;
        return false;
    }

    if (currentUser->getStatus() != UserStatus::ACTIVE) {
        std::cout << "Error: User account '" << currentUser->getUsername() << "' is not active. Cannot request rental." << std::endl;
        return false;
    }

    if (currentUser->getBalance() < 0) {
        std::cout << "Error: User account '" << currentUser->getUsername() 
                  << "' has a negative balance (" << std::fixed << std::setprecision(2) << currentUser->getBalance() 
                  << "). Cannot request new rentals until balance is positive." << std::endl;
        return false;
    }

    Resource* resourceToRent = findResource(resourceId);
    if (!resourceToRent) {
        std::cout << "Error: Resource with ID '" << resourceId << "' not found." << std::endl;
        return false;
    }

    if (resourceToRent->getStatus() != ResourceStatus::IDLE) {
        std::cout << "Error: Resource '" << resourceToRent->getName() << "' is currently not IDLE." << std::endl;
        return false;
    }

    if (durationHours < 1 || durationHours > 15 * 24) {
        std::cout << "Error: Duration must be between 1 hour and 15 days (360 hours). Requested: " << durationHours << " hours." << std::endl;
        return false;
    }

    auto now = std::chrono::system_clock::now();
    auto startTime = now; // Simplified: rental starts immediately upon approval (not implemented yet)
    auto endTime = startTime + std::chrono::hours(durationHours);

    std::string rentalId = generateUniqueId("rental_", rentals.size());
    
    rentals.emplace_back(rentalId, currentUser->getUserId(), resourceId, startTime, endTime);
    std::cout << "Rental request for resource '" << resourceToRent->getName() << "' by user '" << currentUser->getUsername() << "' submitted successfully. Rental ID: " << rentalId << std::endl;
    // Resource status is not changed here; only upon approval.
    return true;
}

std::vector<Rental*> System::getUserRentals(const std::string& userId) {
    std::vector<Rental*> userRentals;
    for (auto& rental : rentals) {
        if (rental.getUserId() == userId) {
            userRentals.push_back(&rental);
        }
    }
    return userRentals;
}

Rental* System::findRental(const std::string& rentalId) {
    auto it = std::find_if(rentals.begin(), rentals.end(),
                           [&rentalId](const Rental& r) { return r.getRentalId() == rentalId; });
    if (it != rentals.end()) {
        return &(*it);
    }
    return nullptr;
}

void System::displayUserRentals(const std::string& userId) { // Should be const
    std::cout << "\n--- Rental History for User ID: " << userId << " ---" << std::endl;
    std::vector<Rental*> userRentals = getUserRentals(userId); // This part is problematic for const
                                                             // If getUserRentals returns Rental* and this method is const
                                                             // then getUserRentals should also be const and return const Rental*
                                                             // For now, keeping as is, but noting this design point.
    if (userRentals.empty()) {
        std::cout << "No rental history found for this user." << std::endl;
    } else {
        for (const auto* rental : userRentals) { // Use const auto* if rental objects are not modified
            rental->displayRentalInfo();
        }
    }
    std::cout << "------------------------------------------" << std::endl;
}

// Rental cancellation
bool System::cancelRentalRequest(const std::string& rentalId) {
    if (!currentUser) {
        std::cout << "Error: No user logged in. Please log in to cancel a rental." << std::endl;
        return false;
    }

    Rental* rentalToCancel = findRental(rentalId);
    if (!rentalToCancel) {
        std::cout << "Error: Rental with ID '" << rentalId << "' not found." << std::endl;
        return false;
    }

    if (rentalToCancel->getUserId() != currentUser->getUserId()) {
        std::cout << "Error: User '" << currentUser->getUsername() 
                  << "' does not own rental '" << rentalId << "'. Cannot cancel." << std::endl;
        return false;
    }

    if (rentalToCancel->getStatus() != RentalStatus::PENDING_APPROVAL) {
        std::cout << "Error: Rental '" << rentalId << "' is not in PENDING_APPROVAL state. Current status: " 
                  << rentalToCancel->rentalStatusToString() << ". Cannot cancel." << std::endl;
        return false;
    }

    rentalToCancel->setStatus(RentalStatus::CANCELLED);
    std::cout << "Rental '" << rentalId << "' cancelled successfully by user '" << currentUser->getUsername() << "'." << std::endl;
    return true;
}

// Admin Resource Management
bool System::adminModifyResource(const std::string& resourceId, const std::string& newName, 
                                 const std::map<std::string, std::string>& newSpecs, double newPricePerHour) {
    if (!currentUser || currentUser->getRole() != UserRole::ADMIN) {
        std::cout << "Error: Admin privileges required to modify resources." << std::endl;
        if(currentUser) std::cout << "Current user: " << currentUser->getUsername() << " Role: " << static_cast<int>(currentUser->getRole()) << std::endl;
        else std::cout << "No user logged in." << std::endl;
        return false;
    }

    Resource* resourceToModify = findResource(resourceId);
    if (!resourceToModify) {
        std::cout << "Error: Resource with ID '" << resourceId << "' not found." << std::endl;
        return false;
    }

    resourceToModify->setName(newName);
    resourceToModify->setSpecs(newSpecs);
    resourceToModify->setPricePerHour(newPricePerHour);

    std::cout << "Resource '" << resourceId << "' modified successfully by admin '" << currentUser->getUsername() << "'." << std::endl;
    return true;
}

bool System::adminDeleteResource(const std::string& resourceId) {
    if (!currentUser || currentUser->getRole() != UserRole::ADMIN) {
        std::cout << "Error: Admin privileges required to delete resources." << std::endl;
        return false;
    }

    Resource* resourceToDelete = findResource(resourceId);
    if (!resourceToDelete) {
        std::cout << "Error: Resource with ID '" << resourceId << "' not found." << std::endl;
        return false;
    }

    // Check if the resource is part of any active or pending rental
    for (const auto& rental : rentals) {
        if (rental.getResourceId() == resourceId) {
            RentalStatus status = rental.getStatus();
            if (status == RentalStatus::ACTIVE || status == RentalStatus::PENDING_APPROVAL || status == RentalStatus::APPROVED) {
                std::cout << "Error: Resource '" << resourceId << "' cannot be deleted. It is part of an active, approved, or pending rental (Rental ID: " 
                          << rental.getRentalId() << ", Status: " << rental.rentalStatusToString() << ")." << std::endl;
                return false;
            }
        }
    }
    
    // If resource is not IDLE but also not in a problematic rental state (e.g. IN_USE but rental is COMPLETED)
    // For simplicity now, we only strictly require IDLE or that it's not in active/pending rentals.
    // A more robust check might be: if (resourceToDelete->getStatus() != ResourceStatus::IDLE) { ... }
    // and then also check rentals. For now, the rental check is primary.

    auto it = std::remove_if(resources.begin(), resources.end(), 
                             [&resourceId](const Resource& r) { return r.getResourceId() == resourceId; });

    if (it != resources.end()) {
        resources.erase(it, resources.end());
        std::cout << "Resource '" << resourceId << "' deleted successfully by admin '" << currentUser->getUsername() << "'." << std::endl;
        return true;
    }

    // Should not happen if findResource found it earlier, but as a fallback:
    std::cout << "Error: Failed to delete resource '" << resourceId << "' (unexpected error during removal)." << std::endl;
    return false; 
}

// Admin User Management
void System::adminDisplayAllUsers() const {
    if (!currentUser || currentUser->getRole() != UserRole::ADMIN) {
        std::cout << "Error: Admin privileges required to display all users." << std::endl;
        return;
    }
    std::cout << "\n--- All Users (Admin View) ---" << std::endl;
    if (users.empty()) {
        std::cout << "No users registered in the system." << std::endl;
    } else {
        for (const auto& user : users) {
            user.displayUserInfo();
        }
    }
    std::cout << "--------------------------------" << std::endl;
}

bool System::adminAddUser(const std::string& username, const std::string& password, UserRole role, const std::string& realName) {
    if (!currentUser || currentUser->getRole() != UserRole::ADMIN) {
        std::cout << "Error: Admin privileges required to add users." << std::endl;
        return false;
    }

    if (findUser(username)) {
        std::cout << "Error: Username '" << username << "' already exists." << std::endl;
        return false;
    }

    std::string userId = generateUniqueId("user_", users.size());
    users.emplace_back(userId, username, password, role, realName);
    std::cout << "User '" << username << "' added successfully by admin '" << currentUser->getUsername() << "' with ID: " << userId << "." << std::endl;
    return true;
}

bool System::adminModifyUser(const std::string& targetUsername, const std::string& newRealName, 
                             UserRole newRole, UserStatus newStatus, double newBalance) {
    if (!currentUser || currentUser->getRole() != UserRole::ADMIN) {
        std::cout << "Error: Admin privileges required to modify users." << std::endl;
        return false;
    }

    User* userToModify = findUser(targetUsername);
    if (!userToModify) {
        std::cout << "Error: User '" << targetUsername << "' not found." << std::endl;
        return false;
    }

    userToModify->setName(newRealName);
    userToModify->setRole(newRole);
    userToModify->setStatus(newStatus);
    userToModify->setBalance(newBalance);

    std::cout << "User '" << targetUsername << "' modified successfully by admin '" << currentUser->getUsername() << "'." << std::endl;

    if (newStatus == UserStatus::SUSPENDED) {
        std::cout << "Note: User '" << targetUsername << "' has been suspended. Their active rentals may need to be managed (e.g., paused or terminated)." << std::endl;
        // Future implementation: Iterate userToModify->getActiveRentals() and update their status.
    }
    return true;
}

bool System::adminSetUserStatus(const std::string& targetUsername, UserStatus newStatus) {
    if (!currentUser || currentUser->getRole() != UserRole::ADMIN) {
        std::cout << "Error: Admin privileges required to set user status." << std::endl;
        return false;
    }

    User* userToModify = findUser(targetUsername);
    if (!userToModify) {
        std::cout << "Error: User '" << targetUsername << "' not found." << std::endl;
        return false;
    }

    userToModify->setStatus(newStatus);
    std::cout << "Status of user '" << targetUsername << "' set to " 
              << (newStatus == UserStatus::ACTIVE ? "ACTIVE" : "SUSPENDED") 
              << " by admin '" << currentUser->getUsername() << "'." << std::endl;

    if (newStatus == UserStatus::SUSPENDED) {
        std::cout << "Note: User '" << targetUsername << "' has been suspended. Their active rentals may need to be managed (e.g., paused or terminated)." << std::endl;
        // Future implementation: Iterate userToModify->getActiveRentals() and update their status.
    }
    return true;
}
