#ifndef BOOKS_H
#define BOOKS_H

#include <pthread.h>

typedef struct {
    char title[100];
    char author[100];
    char isbn[20];
    int copies;
} Book;

typedef enum {
    ADMIN,
    USER
} UserType;

void add_book(const char *isbn, const char *title, const char *author, int copies, char *response);
void delete_book(const char *isbn, char *response);
void modify_book(const char *isbn, const char *title, const char *author, int copies, char *response);
void search_book(const char *isbn, char *response);
void rent_book(const char *isbn, char *response);
void list_books(char *response);
void list_books_by_author(const char *author, char *response);

extern pthread_mutex_t mutex;  // Declare the mutex here

#endif // BOOKS_H
