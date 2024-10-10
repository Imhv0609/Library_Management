#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../header/users.h"

#define USER_FILE "users.dat"

typedef struct {
    char username[50];
    char password[50];
} User;

void register_user() {
    User user;

    printf("Register a new user\n");

    // Check for the username 'admin'
    while (1) {
        printf("Enter new username: ");
        scanf("%s", user.username);
        if (strcmp(user.username, "librarian") == 0) {
            printf("Username 'librarian' is reserved. Please choose a different username.\n");
        } else {
            break;
        }
    }

    printf("Enter new password: ");
    scanf("%s", user.password);

    FILE *file = fopen(USER_FILE, "ab+");
    if (file == NULL) {
        perror("Error opening file for registration");
        return;
    }

    fwrite(&user, sizeof(User), 1, file);
    fclose(file);

    printf("User registered successfully!\n");
}

int login(UserType *userType, int client_socket) {
    char username[50];
    char password[50];
    char response[100];

    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    if (strcmp(username, "librarian") == 0 && strcmp(password, "librarian123") == 0) {
        *userType = ADMIN;
        printf("Logged in as admin.\n");
        return 1;
    }

    // Send username and password to server for verification
    char credentials[150];  // Adjusted buffer size
    snprintf(credentials, sizeof(credentials), "login %s %s", username, password);
    printf("Sending credentials: %s\n", credentials); // Debug message
    write(client_socket, credentials, strlen(credentials));

    // Receive server response
    int bytes_read = read(client_socket, response, sizeof(response) - 1);
    response[bytes_read] = '\0';  // Null-terminate the response
    printf("Received response: %s\n", response); // Debug message

    if (strcmp(response, "success") == 0) {
        *userType = USER;
        printf("Logged in as user.\n");
        return 1;
    } else {
        printf("Invalid username or password. Please try again.\n");
    }

    return 0;
}
