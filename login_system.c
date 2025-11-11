#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define MAX_USERS 100
#define MAX_LOGIN_ATTEMPTS 3
#define PASSWORD_MIN_LENGTH 6

typedef struct {
    char username[50];
    char password[100];
    char email[100];
    int isActive;
    int failedAttempts;
    time_t lastLogin;
} User;

typedef struct {
    User users[MAX_USERS];
    int userCount;
    User* currentUser;
} UserManager;

void initializeUserManager(UserManager* um);
void displayMainMenu();
void registerUser(UserManager* um);
int loginUser(UserManager* um);
void logoutUser(UserManager* um);
void changePassword(UserManager* um);
void resetPassword(UserManager* um);
void displayUserProfile(UserManager* um);
void adminPanel(UserManager* um);
void viewAllUsers(UserManager* um);
void activateDeactivateUser(UserManager* um);
int validatePassword(const char* password);
void encryptPassword(const char* plain, char* encrypted);
void clearInputBuffer();
void pressToContinue();
int isAdmin(UserManager* um);

const char* adminUsername = "admin";

void initializeUserManager(UserManager* um) {
    um->userCount = 0;
    um->currentUser = NULL;

    User admin;
    strcpy(admin.username, "admin");
    encryptPassword("admin123", admin.password);
    strcpy(admin.email, "admin@system.com");
    admin.isActive = 1;
    admin.failedAttempts = 0;
    admin.lastLogin = time(NULL);

    um->users[um->userCount++] = admin;
}

void displayMainMenu() {
    system("clear || cls");
    printf("=== ENHANCED LOGIN SYSTEM ===\n\n");
    printf("1. Register\n");
    printf("2. Login\n");
    printf("3. Forgot Password\n");
    printf("4. User Profile\n");
    printf("5. Change Password\n");
    printf("6. Admin Panel\n");
    printf("7. Logout\n");
    printf("8. Exit\n");
    printf("\nChoose option: ");
}

void registerUser(UserManager* um) {
    if(um->userCount >= MAX_USERS) {
        printf("\n System user limit reached!\n");
        return;
    }

    User newUser;

    printf("\n--- USER REGISTRATION ---\n");

    printf("Username: ");
    scanf("%49s", newUser.username);
    clearInputBuffer();

    for(int i = 0; i < um->userCount; i++) {
        if(strcmp(newUser.username, um->users[i].username) == 0) {
            printf(" Username already exists!\n");
            return;
        }
    }

    char password[50];
    printf("Password (min %d characters): ", PASSWORD_MIN_LENGTH);
    scanf("%49s", password);
    clearInputBuffer();

    if(!validatePassword(password)) {
        printf(" Password must be at least %d characters!\n", PASSWORD_MIN_LENGTH);
        return;
    }

    printf("Email: ");
    scanf("%99s", newUser.email);
    clearInputBuffer();

    encryptPassword(password, newUser.password);
    newUser.isActive = 1;
    newUser.failedAttempts = 0;
    newUser.lastLogin = 0;

    um->users[um->userCount++] = newUser;

    printf(" Registration successful! Welcome %s!\n", newUser.username);
}

int loginUser(UserManager* um) {
    if(um->currentUser != NULL) {
        printf(" Already logged in!\n");
        return 0;
    }

    char username[50];
    char password[50];
    char encrypted[100];

    printf("\n--- LOGIN ---\n");
    printf("Username: ");
    scanf("%49s", username);
    clearInputBuffer();

    printf("Password: ");
    scanf("%49s", password);
    clearInputBuffer();

    for(int i = 0; i < um->userCount; i++) {
        if(strcmp(username, um->users[i].username) == 0) {
            if(!um->users[i].isActive) {
                printf(" Account is deactivated!\n");
                return 0;
            }

            if(um->users[i].failedAttempts >= MAX_LOGIN_ATTEMPTS) {
                printf(" Account locked due to too many failed attempts!\n");
                return 0;
            }

            encryptPassword(password, encrypted);

            if(strcmp(encrypted, um->users[i].password) == 0) {
                um->currentUser = &um->users[i];
                um->currentUser->failedAttempts = 0;
                um->currentUser->lastLogin = time(NULL);

                printf(" Login successful! Welcome back %s!\n", username);
                return 1;
            } else {
                um->users[i].failedAttempts++;
                printf(" Invalid password! Attempts remaining: %d\n",
                       MAX_LOGIN_ATTEMPTS - um->users[i].failedAttempts);
                return 0;
            }
        }
    }

    printf(" User not found!\n");
    return 0;
}

void logoutUser(UserManager* um) {
    if(um->currentUser == NULL) {
        printf(" Not logged in!\n");
        return;
    }

    printf(" Goodbye %s!\n", um->currentUser->username);
    um->currentUser = NULL;
}

void changePassword(UserManager* um) {
    if(um->currentUser == NULL) {
        printf(" Please login first!\n");
        return;
    }

    char current[50], new[50], confirm[50];
    char encrypted[100];

    printf("\n--- CHANGE PASSWORD ---\n");
    printf("Current password: ");
    scanf("%49s", current);
    clearInputBuffer();

    encryptPassword(current, encrypted);
    if(strcmp(encrypted, um->currentUser->password) != 0) {
        printf(" Current password is incorrect!\n");
        return;
    }

    printf("New password: ");
    scanf("%49s", new);
    clearInputBuffer();

    if(!validatePassword(new)) {
        printf(" New password must be at least %d characters!\n", PASSWORD_MIN_LENGTH);
        return;
    }

    printf("Confirm new password: ");
    scanf("%49s", confirm);
    clearInputBuffer();

    if(strcmp(new, confirm) != 0) {
        printf(" Passwords don't match!\n");
        return;
    }

    encryptPassword(new, um->currentUser->password);
    printf(" Password changed successfully!\n");
}

void resetPassword(UserManager* um) {
    char username[50], email[100];

    printf("\n--- PASSWORD RESET ---\n");
    printf("Username: ");
    scanf("%49s", username);
    clearInputBuffer();

    printf("Email: ");
    scanf("%99s", email);
    clearInputBuffer();

    for(int i = 0; i < um->userCount; i++) {
        if(strcmp(username, um->users[i].username) == 0 &&
           strcmp(email, um->users[i].email) == 0) {

            char newPassword[50] = "temp123";
            encryptPassword(newPassword, um->users[i].password);
            um->users[i].failedAttempts = 0;

            printf(" Password reset successful! Temporary password: %s\n", newPassword);
            printf(" Please change your password after login.\n");
            return;
        }
    }

    printf(" User not found or email doesn't match!\n");
}

void displayUserProfile(UserManager* um) {
    if(um->currentUser == NULL) {
        printf(" Please login first!\n");
        return;
    }

    printf("\n--- USER PROFILE ---\n");
    printf("Username: %s\n", um->currentUser->username);
    printf("Email: %s\n", um->currentUser->email);
    printf("Status: %s\n", um->currentUser->isActive ? "Active" : "Inactive");
    printf("Failed attempts: %d\n", um->currentUser->failedAttempts);

    if(um->currentUser->lastLogin > 0) {
        char timeStr[20];
        strftime(timeStr, 20, "%Y-%m-%d %H:%M:%S",
                localtime(&um->currentUser->lastLogin));
        printf("Last login: %s\n", timeStr);
    }
}

void adminPanel(UserManager* um) {
    if(!isAdmin(um)) {
        printf(" Admin access required!\n");
        return;
    }

    int choice;
    do {
        system("clear || cls");
        printf("=== ADMIN PANEL ===\n\n");
        printf("1. View All Users\n");
        printf("2. Activate/Deactivate User\n");
        printf("3. Back to Main Menu\n");
        printf("\nChoose option: ");
        scanf("%d", &choice);
        clearInputBuffer();

        switch(choice) {
            case 1: viewAllUsers(um); break;
            case 2: activateDeactivateUser(um); break;
            case 3: printf("Returning to main menu...\n"); break;
            default: printf(" Invalid choice!\n");
        }
        if(choice != 3) pressToContinue();
    } while(choice != 3);
}

void viewAllUsers(UserManager* um) {
    printf("\n--- ALL USERS (%d) ---\n", um->userCount);
    printf("%-20s %-25s %-8s %-6s\n", "Username", "Email", "Status", "Attempts");
    printf("------------------------------------------------------------\n");

    for(int i = 0; i < um->userCount; i++) {
        printf("%-20s %-25s %-8s %-6d\n",
               um->users[i].username,
               um->users[i].email,
               um->users[i].isActive ? "Active" : "Inactive",
               um->users[i].failedAttempts);
    }
}

void activateDeactivateUser(UserManager* um) {
    char username[50];

    printf("\nUsername to modify: ");
    scanf("%49s", username);
    clearInputBuffer();

    for(int i = 0; i < um->userCount; i++) {
        if(strcmp(username, um->users[i].username) == 0) {
            if(strcmp(username, adminUsername) == 0) {
                printf(" Cannot modify admin account!\n");
                return;
            }

            um->users[i].isActive = !um->users[i].isActive;
            printf(" User %s is now %s\n",
                   username, um->users[i].isActive ? "ACTIVE" : "INACTIVE");
            return;
        }
    }

    printf(" User not found!\n");
}

int validatePassword(const char* password) {
    return strlen(password) >= PASSWORD_MIN_LENGTH;
}

void encryptPassword(const char* plain, char* encrypted) {
    int key = 7;
    int i = 0;

    while(plain[i] != '\0') {
        encrypted[i] = plain[i] + key;
        i++;
    }
    encrypted[i] = '\0';
}

int isAdmin(UserManager* um) {
    return um->currentUser != NULL &&
           strcmp(um->currentUser->username, adminUsername) == 0;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pressToContinue() {
    printf("\nPress Enter to continue...");
    clearInputBuffer();
    getchar();
}

int main() {
    UserManager um;
    initializeUserManager(&um);

    int choice;

    do {
        displayMainMenu();
        scanf("%d", &choice);
        clearInputBuffer();

        switch(choice) {
            case 1: registerUser(&um); break;
            case 2: loginUser(&um); break;
            case 3: resetPassword(&um); break;
            case 4: displayUserProfile(&um); break;
            case 5: changePassword(&um); break;
            case 6: adminPanel(&um); break;
            case 7: logoutUser(&um); break;
            case 8: printf(" Goodbye!\n"); break;
            default: printf(" Invalid choice!\n");
        }

        if(choice != 8) pressToContinue();

    } while(choice != 8);

    return 0;
}
