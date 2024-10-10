#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "../header/books.h"

#define BOOKS_FILE "books.dat"
#define BUFFER_SIZE 1024

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // Initialize the mutex

void add_book(const char *isbn, const char *title, const char *author, int copies, char *response) {
    pthread_mutex_lock(&mutex);  // Lock the mutex

    Book book;
    int found = 0;
    strncpy(book.isbn, isbn, sizeof(book.isbn) - 1);
    strncpy(book.title, title, sizeof(book.title) - 1);
    strncpy(book.author, author, sizeof(book.author) - 1);
    book.copies = copies;

    FILE *file = fopen(BOOKS_FILE, "rb+");
    if (file == NULL) {
        file = fopen(BOOKS_FILE, "wb+");
        if (file == NULL) {
            snprintf(response, BUFFER_SIZE, "Error opening file\n");
            pthread_mutex_unlock(&mutex);  // Unlock the mutex before returning
            return;
        }
    }

    Book temp;
    while (fread(&temp, sizeof(Book), 1, file)) {
        if (strcmp(book.isbn, temp.isbn) == 0) {
            found = 1;
            break;
        }
    }

    if (found) {
        snprintf(response, BUFFER_SIZE, "Book with ISBN %s already exists. Use the modify book function.\n", book.isbn);
        fclose(file);
        pthread_mutex_unlock(&mutex);  // Unlock the mutex before returning
        return;
    }

    fseek(file, 0, SEEK_END);
    sleep(5);  // Simulate a delay
    fwrite(&book, sizeof(Book), 1, file);
    fclose(file);

    pthread_mutex_unlock(&mutex);  // Unlock the mutex
    snprintf(response, BUFFER_SIZE, "Book added successfully!\n");
}

void rent_book(const char *isbn, char *response) {
    pthread_mutex_lock(&mutex);  // Lock the mutex

    FILE *file = fopen(BOOKS_FILE, "rb+");
    if (file == NULL) {
        snprintf(response, BUFFER_SIZE, "Error opening file\n");
        pthread_mutex_unlock(&mutex);  // Unlock the mutex before returning
        return;
    }

    Book book;
    int found = 0;

    while (fread(&book, sizeof(Book), 1, file)) {
        if (strcmp(isbn, book.isbn) == 0) {
            found = 1;
            break;
        }
    }

    if (found) {
        if (book.copies > 0) {
            book.copies--;
            fseek(file, -sizeof(Book), SEEK_CUR);
            sleep(5);  // Simulate a delay
            fwrite(&book, sizeof(Book), 1, file);
            snprintf(response, BUFFER_SIZE, "Book rented successfully!\n");
        } else {
            snprintf(response, BUFFER_SIZE, "No copies available for rent.\n");
        }
    } else {
        snprintf(response, BUFFER_SIZE, "Book not found.\n");
    }

    fclose(file);
    pthread_mutex_unlock(&mutex);  // Unlock the mutex
}

void delete_book(const char *isbn, char *response) {
    pthread_mutex_lock(&mutex);  // Lock the mutex

    FILE *file = fopen(BOOKS_FILE, "rb");
    FILE *temp = fopen("temp.dat", "wb");

    if (file == NULL || temp == NULL) {
        snprintf(response, BUFFER_SIZE, "Error opening file\n");
        pthread_mutex_unlock(&mutex);  // Unlock the mutex before returning
        return;
    }

    Book book;
    int found = 0;

    while (fread(&book, sizeof(Book), 1, file)) {
        if (strcmp(isbn, book.isbn) != 0) {
            fwrite(&book, sizeof(Book), 1, temp);
        } else {
            found = 1;
        }
    }

    sleep(5);  // Simulate a delay
    fclose(file);
    fclose(temp);

    remove(BOOKS_FILE);
    rename("temp.dat", BOOKS_FILE);

    if (found) {
        snprintf(response, BUFFER_SIZE, "Book deleted successfully!\n");
    } else {
        snprintf(response, BUFFER_SIZE, "Book not found.\n");
    }

    pthread_mutex_unlock(&mutex);  // Unlock the mutex
}

void modify_book(const char *isbn, const char *title, const char *author, int copies, char *response) {
    pthread_mutex_lock(&mutex);  // Lock the mutex

    FILE *file = fopen(BOOKS_FILE, "rb+");
    if (file == NULL) {
        snprintf(response, BUFFER_SIZE, "Error opening file\n");
        pthread_mutex_unlock(&mutex);  // Unlock the mutex before returning
        return;
    }

    Book book;
    int found = 0;

    while (fread(&book, sizeof(Book), 1, file)) {
        if (strcmp(isbn, book.isbn) == 0) {
            found = 1;
            strncpy(book.title, title, sizeof(book.title) - 1);
            strncpy(book.author, author, sizeof(book.author) - 1);
            book.copies = copies;
            fseek(file, -sizeof(Book), SEEK_CUR);
            sleep(5);  // Simulate a delay
            fwrite(&book, sizeof(Book), 1, file);
            break;
        }
    }

    fclose(file);
    if (found) {
        snprintf(response, BUFFER_SIZE, "Book modified successfully!\n");
    } else {
        snprintf(response, BUFFER_SIZE, "Book not found.\n");
    }

    pthread_mutex_unlock(&mutex);  // Unlock the mutex
}

void search_book(const char *isbn, char *response) {
    pthread_mutex_lock(&mutex);  // Lock the mutex

    FILE *file = fopen(BOOKS_FILE, "rb");
    if (file == NULL) {
        snprintf(response, BUFFER_SIZE, "Error opening file\n");
        pthread_mutex_unlock(&mutex);  // Unlock the mutex before returning
        return;
    }

    Book book;
    int found = 0;

    while (fread(&book, sizeof(Book), 1, file)) {
        if (strcmp(isbn, book.isbn) == 0) {
            snprintf(response, BUFFER_SIZE, "Book found: ISBN: %s, Title: %s, Author: %s, Copies: %d\n", book.isbn, book.title, book.author, book.copies);
            found = 1;
            break;
        }
    }

    fclose(file);
    if (!found) {
        snprintf(response, BUFFER_SIZE, "Book not found.\n");
    }

    pthread_mutex_unlock(&mutex);  // Unlock the mutex
}

void list_books(char *response) {
    pthread_mutex_lock(&mutex);  // Lock the mutex

    FILE *file = fopen(BOOKS_FILE, "rb");
    if (file == NULL) {
        snprintf(response, BUFFER_SIZE, "Error opening file\n");
        pthread_mutex_unlock(&mutex);  // Unlock the mutex before returning
        return;
    }

    Book book;
    int count = 0;
    char temp_response[BUFFER_SIZE * 10];
    strcpy(temp_response, "List of all books:\n");

    while (fread(&book, sizeof(Book), 1, file)) {
        char book_info[BUFFER_SIZE];
        snprintf(book_info, BUFFER_SIZE, "ISBN: %s, Title: %s, Author: %s, Copies: %d\n", book.isbn, book.title, book.author, book.copies);
        strcat(temp_response, book_info);
        count++;
    }

    fclose(file);
    if (count == 0) {
        snprintf(response, BUFFER_SIZE, "No books found in the library.\n");
    } else {
        strncpy(response, temp_response, BUFFER_SIZE - 1);
    }

    pthread_mutex_unlock(&mutex);  // Unlock the mutex
}

void list_books_by_author(const char *author, char *response) {
    pthread_mutex_lock(&mutex);  // Lock the mutex

    FILE *file = fopen(BOOKS_FILE, "rb");
    if (file == NULL) {
        snprintf(response, BUFFER_SIZE, "Error opening file\n");
        pthread_mutex_unlock(&mutex);  // Unlock the mutex before returning
        return;
    }

    Book book;
    int count = 0;
    char temp_response[BUFFER_SIZE * 10];
    snprintf(temp_response, sizeof(temp_response), "List of books by %s:\n", author);

    while (fread(&book, sizeof(Book), 1, file)) {
        if (strcmp(book.author, author) == 0) {
            char book_info[BUFFER_SIZE];
            snprintf(book_info, BUFFER_SIZE, "ISBN: %s, Title: %s, Author: %s, Copies: %d\n", book.isbn, book.title, book.author, book.copies);
            strcat(temp_response, book_info);
            count++;
        }
    }

    fclose(file);
    if (count == 0) {
        snprintf(response, BUFFER_SIZE, "No books found by author %s.\n", author);
    } else {
        strncpy(response, temp_response, BUFFER_SIZE - 1);
    }

    pthread_mutex_unlock(&mutex);  // Unlock the mutex
}
