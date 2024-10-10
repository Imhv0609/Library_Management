#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../header/users.h"

#define PORT 8020
#define BUFFER_SIZE 1024

void send_command(int socket, const char *command) {
    write(socket, command, strlen(command));
}

void receive_response(int socket) {
    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    read(socket, buffer, BUFFER_SIZE);
    printf("%s", buffer);
}

void admin_menu(int socket) {
    int choice;
    char buffer[BUFFER_SIZE];
    char isbn[20];
    char title[100];
    char author[100];
    int copies;

    do {
        printf("\nLibrary Management System - Admin Menu\n");
        printf("1. Add Book\n");
        printf("2. Rent Book\n");
        printf("3. Delete Book\n");
        printf("4. Modify Book\n");
        printf("5. Search Book\n");
        printf("6. List All Books\n");
        printf("7. List Books by Author\n");
        printf("8. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter ISBN: ");
                scanf("%s", isbn);
                printf("Enter Title: ");
                scanf(" %[^\n]%*c", title);
                printf("Enter Author: ");
                scanf(" %[^\n]%*c", author);
                printf("Enter Number of Copies: ");
                scanf("%d", &copies);
                snprintf(buffer, BUFFER_SIZE, "add %s\t%s\t%s\t%d", isbn, title, author, copies);
                send_command(socket, buffer);
                receive_response(socket);
                break;
            case 2:
                printf("Enter ISBN: ");
                scanf("%s", isbn);
                snprintf(buffer, BUFFER_SIZE, "rent %s", isbn);
                send_command(socket, buffer);
                receive_response(socket);
                break;
            case 3:
                printf("Enter ISBN: ");
                scanf("%s", isbn);
                snprintf(buffer, BUFFER_SIZE, "delete %s", isbn);
                send_command(socket, buffer);
                receive_response(socket);
                break;
            case 4:
                printf("Enter ISBN: ");
                scanf("%s", isbn);
                printf("Enter New Title: ");
                scanf(" %[^\n]%*c", title);
                printf("Enter New Author: ");
                scanf(" %[^\n]%*c", author);
                printf("Enter New Number of Copies: ");
                scanf("%d", &copies);
                snprintf(buffer, BUFFER_SIZE, "modify %s\t%s\t%s\t%d", isbn, title, author, copies);
                send_command(socket, buffer);
                receive_response(socket);
                break;
            case 5:
                printf("Enter ISBN: ");
                scanf("%s", isbn);
                snprintf(buffer, BUFFER_SIZE, "search %s", isbn);
                send_command(socket, buffer);
                receive_response(socket);
                break;
            case 6:
                send_command(socket, "list");
                receive_response(socket);
                break;
            case 7:
                printf("Enter Author: ");
                scanf(" %[^\n]%*c", author);
                snprintf(buffer, BUFFER_SIZE, "listbyauthor %s", author);
                send_command(socket, buffer);
                receive_response(socket);
                break;
            case 8:
                send_command(socket, "logout");
                printf("Logging out...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 8);
}

void user_menu(int socket) {
    int choice;
    char buffer[BUFFER_SIZE];
    char isbn[20];
    char author[100];

    do {
        printf("\nLibrary Management System - User Menu\n");
        printf("1. Rent Book\n");
        printf("2. Search Book\n");
        printf("3. List All Books\n");
        printf("4. List Books by Author\n");
        printf("5. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter ISBN: ");
                scanf("%s", isbn);
                snprintf(buffer, BUFFER_SIZE, "rent %s", isbn);
                send_command(socket, buffer);
                receive_response(socket);
                break;
            case 2:
                printf("Enter ISBN: ");
                scanf("%s", isbn);
                snprintf(buffer, BUFFER_SIZE, "search %s", isbn);
                send_command(socket, buffer);
                receive_response(socket);
                break;
            case 3:
                send_command(socket, "list");
                receive_response(socket);
                break;
            case 4:
                printf("Enter Author: ");
                scanf(" %[^\n]%*c", author);
                snprintf(buffer, BUFFER_SIZE, "listbyauthor %s", author);
                send_command(socket, buffer);
                receive_response(socket);
                break;
            case 5:
                send_command(socket, "logout");
                printf("Logging out...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 5);
}

void user_options(int socket) {
    int choice;
    UserType userType;
    int login_success = 0;

    do {
        printf("\n1. Register as new user\n");
        printf("2. Login as existing user\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            register_user();
            printf("Please login with your new credentials.\n");
        }

        login_success = login(&userType, socket);
    } while (!login_success);

    if (userType == ADMIN) {
        admin_menu(socket);
    } else {
        user_menu(socket);
    }
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        perror("Connection to the server failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server\n");

    user_options(client_socket);

    close(client_socket);
    return 0;
}
