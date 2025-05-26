#include "System.h" 
#include "User.h"   
#include "Resource.h" // For Resource and ResourceType
#include "Rental.h"   // For Rental and RentalStatus
#include "Bill.h"     // For Bill
#include <iostream>
#include <iomanip>   // For std::fixed and std::setprecision
#include <map>     // For resource specs
#include <chrono>  // For std::chrono for time manipulations in main (if needed)

int main() {
    System sys;

    std::cout << "--- Initial User and Resource Setup for Billing Tests ---" << std::endl;
    // Register users
    sys.registerUser("alice_b", "pass123", UserRole::STUDENT, "Alice Billing");
    sys.registerUser("bob_b", "securePwd", UserRole::TEACHER, "Bob Billing");
    sys.registerUser("admin01", "adminPass", UserRole::ADMIN, "Sys Admin");

    // Add resources
    sys.addResource(Resource("cpu_bill_01", ResourceType::CPU, "Billing CPU 1", {{"Cores", "2"}}, 10.0)); // $10/hr
    sys.addResource(Resource("gpu_bill_01", ResourceType::GPU, "Billing GPU 1", {{"Memory", "4GB"}}, 25.0)); // $25/hr

    // Admin sets initial balances
    User* admin_login = sys.loginUser("admin01", "adminPass");
    if (admin_login) {
        std::cout << "Admin logged in to set initial balances." << std::endl;
        sys.adminModifyUser("alice_b", "Alice Billing", UserRole::STUDENT, UserStatus::ACTIVE, 50.0); // Alice gets $50
        sys.adminModifyUser("bob_b", "Bob Billing", UserRole::TEACHER, UserStatus::ACTIVE, 15.0);   // Bob gets $15
        sys.logoutUser();
    } else {
        std::cout << "CRITICAL ERROR: Admin login failed during setup." << std::endl;
        return 1;
    }

    std::cout << "\n--- Test Case 1: Alice - Successful Rental & Billing ---" << std::endl;
    User* alice = sys.loginUser("alice_b", "pass123");
    std::string alices_rental_id = "";
    if (alice) {
        std::cout << "Alice logged in. Initial Balance: $" << alice->getBalance() << std::endl;
        // Alice requests cpu_bill_01 for 2 hours (Cost: 2 * $10 = $20)
        if (sys.requestResourceRental("cpu_bill_01", 2)) {
            auto rentals = sys.getUserRentals(alice->getUserId());
            if (!rentals.empty()) alices_rental_id = rentals.back()->getRentalId();
            std::cout << "Alice's rental request for 'cpu_bill_01' (ID: " << alices_rental_id << ") submitted." << std::endl;
        } else {
            std::cout << "Alice failed to request 'cpu_bill_01'." << std::endl;
        }
        sys.logoutUser();
    } else {
        std::cout << "Failed to log in Alice for Test Case 1." << std::endl;
    }

    // Admin approves Alice's rental
    admin_login = sys.loginUser("admin01", "adminPass");
    if (admin_login && !alices_rental_id.empty()) {
        std::cout << "Admin logged in to approve Alice's rental." << std::endl;
        if (sys.adminApproveRental(alices_rental_id)) {
            std::cout << "Admin approved Alice's rental " << alices_rental_id << "." << std::endl;
        } else {
            std::cout << "Admin failed to approve Alice's rental " << alices_rental_id << "." << std::endl;
        }
        sys.logoutUser();
    } else {
         std::cout << "Admin login failed or Alice's rental ID missing for approval." << std::endl;
    }
    
    // Simulate rental completion for Alice (can be called by anyone, but logically by system/admin)
    // No need to log in admin if processRentalCompletion doesn't check role (it currently doesn't)
    std::cout << "\nProcessing completion for Alice's rental: " << alices_rental_id << std::endl;
    if (sys.processRentalCompletion(alices_rental_id)) {
        std::cout << "Alice's rental " << alices_rental_id << " processed for completion." << std::endl;
    } else {
        std::cout << "Failed to process completion for Alice's rental " << alices_rental_id << "." << std::endl;
    }

    // Verify Alice's state
    alice = sys.loginUser("alice_b", "pass123");
    if (alice) {
        std::cout << "\nAlice logged in to verify status after rental." << std::endl;
        std::cout << "Alice's final Balance: $" << std::fixed << std::setprecision(2) << alice->getBalance() << " (Expected: $30.00)" << std::endl;
        sys.displayUserBills(alice->getUserId());
        
        Rental* r_alice = sys.findRental(alices_rental_id); // findRental is public
        if(r_alice) std::cout << "Alice's rental status: " << r_alice->rentalStatusToString() << " (Expected: COMPLETED)" << std::endl;
        
        Resource* res_alice = sys.findResource("cpu_bill_01"); // findResource is public
        if(res_alice) std::cout << "Resource 'cpu_bill_01' status: " << (res_alice->getStatus() == ResourceStatus::IDLE ? "Idle" : "In Use") << " (Expected: Idle)" << std::endl;
        sys.logoutUser();
    }

    std::cout << "\n--- Test Case 2: Bob - Insufficient Balance leading to Negative ---" << std::endl;
    User* bob = sys.loginUser("bob_b", "securePwd");
    std::string bobs_rental_id = "";
    if (bob) {
        std::cout << "Bob logged in. Initial Balance: $" << bob->getBalance() << std::endl;
        // Bob requests gpu_bill_01 for 1 hour (Cost: 1 * $25 = $25. Bob has $15)
        if (sys.requestResourceRental("gpu_bill_01", 1)) {
            auto rentals = sys.getUserRentals(bob->getUserId());
            if (!rentals.empty()) bobs_rental_id = rentals.back()->getRentalId();
            std::cout << "Bob's rental request for 'gpu_bill_01' (ID: " << bobs_rental_id << ") submitted." << std::endl;
        } else {
            std::cout << "Bob failed to request 'gpu_bill_01'." << std::endl;
        }
        sys.logoutUser();
    } else {
        std::cout << "Failed to log in Bob for Test Case 2." << std::endl;
    }

    // Admin approves Bob's rental
    admin_login = sys.loginUser("admin01", "adminPass");
    if (admin_login && !bobs_rental_id.empty()) {
        std::cout << "Admin logged in to approve Bob's rental." << std::endl;
        sys.adminApproveRental(bobs_rental_id);
        sys.logoutUser();
    } else {
        std::cout << "Admin login failed or Bob's rental ID missing for approval." << std::endl;
    }

    // Simulate rental completion for Bob
    std::cout << "\nProcessing completion for Bob's rental: " << bobs_rental_id << std::endl;
    sys.processRentalCompletion(bobs_rental_id);

    // Verify Bob's state and attempt new rental
    bob = sys.loginUser("bob_b", "securePwd");
    if (bob) {
        std::cout << "\nBob logged in to verify status after rental." << std::endl;
        std::cout << "Bob's final Balance: $" << bob->getBalance() << " (Expected: -$10.00)" << std::endl;
        sys.displayUserBills(bob->getUserId());

        std::cout << "\nBob attempting to rent 'cpu_bill_01' with negative balance:" << std::endl;
        if (!sys.requestResourceRental("cpu_bill_01", 1)) {
            std::cout << "Correctly failed: Bob cannot rent with negative balance." << std::endl;
        } else {
            std::cout << "ERROR: Bob was able to rent with negative balance." << std::endl;
        }
        sys.logoutUser();
    }

    std::cout << "\n--- Test Case 3: Admin Display All Bills ---" << std::endl;
    admin_login = sys.loginUser("admin01", "adminPass");
    if (admin_login) {
        std::cout << "Admin logged in." << std::endl;
        sys.adminDisplayAllBills();
        sys.logoutUser();
    } else {
        std::cout << "Admin login failed for displaying all bills." << std::endl;
    }
    
    std::cout << "\n--- Testing Complete ---" << std::endl;
    return 0;
}
