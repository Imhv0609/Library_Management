# Library Management System

A multi-threaded client-server library management system implemented in C with socket programming and mutex-based concurrency control.

## Project Overview

This project implements a distributed library management system where:
- **Server**: Handles multiple concurrent client connections using pthread threading
- **Client**: Provides interactive menu-driven interface for users and administrators
- **Data Storage**: Uses binary files for persistent storage of books and user data
- **Concurrency**: Thread-safe operations using pthread mutexes
- **Authentication**: Role-based access control (Admin/User)

## Features

### Administrator Features
- Add new books to the library
- Delete existing books
- Modify book information (title, author, copies)
- Rent books to users
- Search for books by ISBN
- List all books in the library
- List books by specific author
- Full administrative access

### User Features
- Search for books by ISBN
- Rent available books
- List all books in the library
- List books by specific author
- Limited access (no add/delete/modify operations)

### System Features
- **Multi-threading**: Server handles multiple clients simultaneously
- **Thread Safety**: Mutex locks ensure data integrity during concurrent operations
- **Persistent Storage**: Binary file storage for books and users
- **Role-based Access**: Different privileges for admin and regular users
- **Network Communication**: TCP socket-based client-server architecture
- **Input Validation**: Handles various edge cases and invalid inputs

## Architecture

### File Structure
```
├── header/
│   ├── books.h          # Book operations and data structures
│   └── users.h          # User authentication and management
├── src/
│   ├── server.c         # Server implementation with threading
│   ├── client.c         # Client interface and menu system
│   ├── books.c          # Book management functions
│   └── users.c          # User authentication functions
├── books.dat            # Binary file storing book data
├── users.dat           # Binary file storing user credentials
├── Makefile            # Build configuration
└── README.md           # This file
```

### Data Structures

#### Book Structure
```c
typedef struct {
    char title[100];      // Book title
    char author[100];     // Author name
    char isbn[20];        // ISBN (unique identifier)
    int copies;           // Number of available copies
} Book;
```

#### User Structure
```c
typedef struct {
    char username[50];    // Username
    char password[50];    // Password
} User;
```

## Prerequisites

- GCC compiler with pthread support
- UNIX-like operating system (Linux, macOS, etc.)
- Network connectivity (localhost for testing)

## Installation & Setup

1. **Clone or download the project files**
   ```bash
   # Navigate to project directory
   cd IMT2022101_OS_MIN_PROJECT
   ```

2. **Compile the project**
   ```bash
   make clean    # Clean previous builds
   make          # Compile server and client
   ```

3. **Verify compilation**
   ```bash
   ls -la
   # Should show 'server' and 'client' executables
   ```

## Usage

### Starting the System

1. **Start the Server** (in terminal 1)
   ```bash
   ./server
   ```
   Output: `Server listening on port 8020`

2. **Start the Client** (in terminal 2)
   ```bash
   ./client
   ```

### Authentication

#### Default Admin Credentials
- **Username**: `librarian`
- **Password**: `librarian123`
- **Access**: Full administrative privileges

#### User Registration
1. Select option `1` to register as new user
2. Choose any username except `librarian` (reserved)
3. Set a password
4. Login with your new credentials

#### User Login Process
- Client sends credentials to server
- Server verifies against `users.dat`
- Server prompts for re-verification (security feature)
- Access granted based on role (Admin/User)

### Menu Operations

#### Admin Menu
```
1. Add Book       - Add new book to library
2. Rent Book      - Process book rental
3. Delete Book    - Remove book from library
4. Modify Book    - Update book information
5. Search Book    - Find book by ISBN
6. List All Books - Display all books
7. List Books by Author - Filter by author
8. Logout         - Exit session
```

#### User Menu
```
1. Rent Book      - Process book rental
2. Search Book    - Find book by ISBN
3. List All Books - Display all books
4. List Books by Author - Filter by author
5. Logout         - Exit session
```

### Example Usage Session

1. **Register a new user**
   ```
   1. Register as new user
   2. Login as existing user
   Enter your choice: 1
   
   Register a new user
   Enter new username: john_doe
   Enter new password: mypassword
   User registered successfully!
   ```

2. **Login as admin**
   ```
   Enter your choice: 2
   Enter username: librarian
   Enter password: librarian123
   Re-enter username: librarian
   Re-enter password: librarian123
   Logged in as admin.
   ```

3. **Add a book**
   ```
   Enter your choice: 1
   Enter ISBN: 978-0134685991
   Enter Title: Effective Modern C++
   Enter Author: Scott Meyers
   Enter Number of Copies: 5
   Book added successfully!
   ```

## Technical Details

### Concurrency Control
- **Pthread Mutexes**: Ensure thread-safe access to shared resources
- **File Locking**: Prevents data corruption during concurrent file operations
- **Atomic Operations**: Book add/modify/delete operations are atomic

### Network Communication
- **Protocol**: TCP sockets
- **Port**: 8020 (configurable in source)
- **Message Format**: Command-based text protocol
- **Response Handling**: Synchronous request-response model

### Error Handling
- File I/O error checking
- Network connection error handling
- Invalid input validation
- Resource cleanup on errors

### Performance Considerations
- **Simulated Delays**: 5-second delays in critical operations (can be removed for production)
- **Buffer Management**: Proper buffer sizing for network communication
- **Memory Management**: Dynamic memory allocation for client threads

## Configuration

### Modifying Server Port
Edit `server.c` and `client.c`:
```c
#define PORT 8020  // Change to desired port
```

### Adjusting Buffer Sizes
Edit relevant source files:
```c
#define BUFFER_SIZE 1024  // Increase for larger data
```

### Removing Simulated Delays
Comment out or remove `sleep(5);` calls in `books.c` for faster operations.

## Troubleshooting

### Common Issues

1. **Compilation Errors**
   - Ensure GCC supports pthread: `gcc --version`
   - Install build essentials if missing

2. **Server Won't Start**
   - Port 8020 might be in use: `lsof -i :8020`
   - Kill existing processes: `pkill -f server`

3. **Client Connection Failed**
   - Ensure server is running first
   - Check firewall settings
   - Verify server is listening: `netstat -an | grep 8020`

4. **File Permission Errors**
   - Ensure write permissions: `chmod 644 books.dat users.dat`
   - Run from directory containing data files

5. **Threading Issues**
   - Verify pthread library linking
   - Check system thread limits: `ulimit -u`

### Debug Mode
Add debug prints in source code:
```c
printf("Debug: %s\n", debug_message);
```

## Data File Format

- **books.dat**: Binary file containing Book structures
- **users.dat**: Binary file containing User structures
- **temp.dat**: Temporary file used during delete operations

**Note**: Data files are created automatically if they don't exist.

## Security Considerations

- Passwords stored in plain text (not recommended for production)
- Server-side re-authentication required for sensitive operations
- No encryption for network communication
- Consider implementing SSL/TLS for production use

## Future Enhancements

1. **Security**
   - Password hashing (bcrypt, scrypt)
   - SSL/TLS encryption
   - Session tokens

2. **Features**
   - Return book functionality
   - Book reservation system
   - Late fee calculation
   - Search by title/author

3. **Performance**
   - Database integration (SQLite, MySQL)
   - Connection pooling
   - Caching mechanisms

4. **User Interface**
   - Web-based interface
   - GUI client application
   - Mobile app support

## License

This project is created for educational purposes as part of Operating Systems coursework (IMT2022101).

## Author

Student ID: IMT2022101
Course: Operating Systems
Project: Minimum OS Project - Library Management System

---

For questions or issues, please refer to the source code comments or contact the development team.