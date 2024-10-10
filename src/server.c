#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "../header/books.h"

#define PORT 8020
#define BUFFER_SIZE 1024
#define USER_FILE "users.dat"

typedef struct {
    char username[50];
    char password[50];
} User;

void send_response(int client_socket, const char *response) {
    write(client_socket, response, strlen(response));
    write(client_socket, "\0", 1);  // Add null terminator to response
}

void prompt_server_login(char *username, char *password) {
    printf("Server-side verification required.\n");

    printf("Re-enter username: ");
    scanf("%s", username);
    printf("Re-enter password: ");
    scanf("%s", password);
}

void handle_login(int client_socket, char *username, char *password) {
    FILE *file = fopen(USER_FILE, "rb");
    if (file == NULL) {
        perror("Error opening file for login");
        send_response(client_socket, "fail");
        return;
    }

    User user;
    int user_found = 0;
    while (fread(&user, sizeof(User), 1, file)) {
        if (strcmp(username, user.username) == 0 && strcmp(password, user.password) == 0) {
            user_found = 1;
            break;
        }
    }
    fclose(file);

    if (user_found) {
        char server_username[50], server_password[50];
        prompt_server_login(server_username, server_password);

        if (strcmp(username, server_username) == 0 && strcmp(password, server_password) == 0) {
            send_response(client_socket, "success");
        } else {
            send_response(client_socket, "fail");
        }
    } else {
        send_response(client_socket, "fail");
    }
}

void* handle_client(void *arg) {
    int client_socket = *(int*)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    char command[BUFFER_SIZE];
    char isbn[20];
    char title[100];
    char author[100];
    int copies;
    char response[BUFFER_SIZE];

    while (1) {
        bzero(buffer, BUFFER_SIZE);
        int bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1);
        buffer[bytes_read] = '\0';

        // Extract the command
        sscanf(buffer, "%s", command);

        if (strncmp("login", command, 5) == 0) {
            char username[50], password[50];
            sscanf(buffer + 6, "%s %s", username, password);
            handle_login(client_socket, username, password);
        } else if (strncmp("add", command, 3) == 0) {
            // Extract parameters for add
            sscanf(buffer + 4, "%19s %99[^\t] %99[^\t] %d", isbn, title, author, &copies);
            add_book(isbn, title, author, copies, response);
        } else if (strncmp("rent", command, 4) == 0) {
            // Extract parameters for rent
            sscanf(buffer + 5, "%19s", isbn);
            rent_book(isbn, response);
        } else if (strncmp("delete", command, 6) == 0) {
            // Extract parameters for delete
            sscanf(buffer + 7, "%19s", isbn);
            delete_book(isbn, response);
        } else if (strncmp("modify", command, 6) == 0) {
            // Extract parameters for modify
            sscanf(buffer + 7, "%19s %99[^\t] %99[^\t] %d", isbn, title, author, &copies);
            modify_book(isbn, title, author, copies, response);
        } else if (strncmp("search", command, 6) == 0) {
            // Extract parameters for search
            sscanf(buffer + 7, "%19s", isbn);
            search_book(isbn, response);
        } else if (strncmp("list", command, 4) == 0) {
            // List all books
            list_books(response);
        } else if (strncmp("listbyauthor", command, 12) == 0) {
            // Extract parameters for list by author
            sscanf(buffer + 13, "%99[^\t]", author);
            list_books_by_author(author, response);
        } else if (strncmp("logout", command, 6) == 0) {
            printf("Client logged out.\n");
            break;
        } else {
            snprintf(response, BUFFER_SIZE, "Invalid command.\n");
        }

        send_response(client_socket, response);
    }
    close(client_socket);
    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) != 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Server accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Client connected\n");

        pthread_t thread;
        int *pclient = malloc(sizeof(int));
        *pclient = client_socket;
        pthread_create(&thread, NULL, handle_client, pclient);
        pthread_detach(thread);
    }

    close(server_socket);
    return 0;
}
