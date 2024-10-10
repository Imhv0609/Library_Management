#ifndef USERS_H
#define USERS_H

typedef enum {
    ADMIN,
    USER
} UserType;

int login(UserType *userType, int client_socket);
void register_user();

#endif // USERS_H
