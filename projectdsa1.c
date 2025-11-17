//DSA PROJECT (Library Management System)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOOK_TABLE_SIZE 101
#define MEMBER_TABLE_SIZE 101


struct Book {
    int id;
    char title[100];
    char author[100];
    int totalCopies;
    int availableCopies;
    struct Book* next; 
};

struct Member {
    int id;
    char name[100];
    int issuedBookId;
    struct Member* next; 
};

struct Library {
    struct Book* bookTable[BOOK_TABLE_SIZE];
    struct Member* memberTable[MEMBER_TABLE_SIZE];
};

int hash(int key, int size) {
    return key % size;
}


void addBook(struct Library* lib, int id, char title[], char author[], int copies) {
    // Check if book ID already exists (Simple check within the same bucket)
    if (getBook(lib, id) != NULL) {
        printf("Error: Book with ID %d already exists. Not adding '%s'.\n", id, title);
        return;
    }

    int index = hash(id, BOOK_TABLE_SIZE);
    struct Book* newBook = (struct Book*) malloc(sizeof(struct Book));
    if (newBook == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    newBook->id = id;
    strcpy(newBook->title, title);
    strcpy(newBook->author, author);
    newBook->totalCopies = copies;
    newBook->availableCopies = copies;
    newBook->next = NULL;

    // Add to the front of the linked list (chaining)
    newBook->next = lib->bookTable[index];
    lib->bookTable[index] = newBook;

    printf("Book '%s' added successfully.\n", title);
}

struct Book* getBook(struct Library* lib, int id) {
    int index = hash(id, BOOK_TABLE_SIZE);
    struct Book* temp = lib->bookTable[index];
    while (temp) {
        if (temp->id == id)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

struct Book* searchBookByTitle(struct Library* lib, char title[]) {
    // Search is case-sensitive and finds the first partial match (using strstr)
    for (int i = 0; i < BOOK_TABLE_SIZE; i++) {
        struct Book* temp = lib->bookTable[i];
        while (temp) {
            if (strstr(temp->title, title))
                return temp;
            temp = temp->next;
        }
    }
    return NULL;
}


void addMember(struct Library* lib, int id, char name[]) {
    // Check if member ID already exists (Simple check within the same bucket)
    if (getMember(lib, id) != NULL) {
        printf("Error: Member with ID %d already exists. Not adding '%s'.\n", id, name);
        return;
    }

    int index = hash(id, MEMBER_TABLE_SIZE);
    struct Member* newMember = (struct Member*) malloc(sizeof(struct Member));
    if (newMember == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    newMember->id = id;
    strcpy(newMember->name, name);
    newMember->issuedBookId = -1; // -1 means no book issued
    newMember->next = NULL;

    // Add to the front of the linked list (chaining)
    newMember->next = lib->memberTable[index];
    lib->memberTable[index] = newMember;

    printf("Member '%s' added successfully.\n", name);
}

struct Member* getMember(struct Library* lib, int id) {
    int index = hash(id, MEMBER_TABLE_SIZE);
    struct Member* temp = lib->memberTable[index];
    while (temp) {
        if (temp->id == id)
            return temp;
        temp = temp->next;
    }
    return NULL;
}


void issueBook(struct Library* lib, int memberId, int bookId) {
    struct Member* member = getMember(lib, memberId);
    struct Book* book = getBook(lib, bookId);

    if (!member || !book) {
        printf("Invalid member or book ID.\n");
        return;
    }

    if (book->availableCopies > 0 && member->issuedBookId == -1) {
        book->availableCopies--;
        member->issuedBookId = bookId;
        printf("Book '%s' issued to member '%s'.\n", book->title, member->name);
    } else {
        if (book->availableCopies == 0) {
            printf("Cannot issue book. '%s' is currently unavailable.\n", book->title);
        } else { // member->issuedBookId != -1
            printf("Cannot issue book. Member '%s' already has a book issued (ID: %d).\n", member->name, member->issuedBookId);
        }
    }
}

void returnBook(struct Library* lib, int memberId) {
    struct Member* member = getMember(lib, memberId);

    if (!member) {
        printf("Member not found.\n");
        return;
    }

    if (member->issuedBookId == -1) {
        printf("No book to return for member '%s'.\n", member->name);
        return;
    }

    struct Book* book = getBook(lib, member->issuedBookId);
    if (book) {
        book->availableCopies++;
        printf("Book '%s' returned by '%s'.\n", book->title, member->name);
    } else {
        // This case is unlikely if issueBook works correctly, but handles data inconsistency
        printf("Error: Book with ID %d issued to member '%s' was not found in the library records.\n", member->issuedBookId, member->name);
    }

    member->issuedBookId = -1;
}


void displayBooks(struct Library* lib) {
    printf("\n--- Books in Library ---\n");
    for (int i = 0; i < BOOK_TABLE_SIZE; i++) {
        struct Book* temp = lib->bookTable[i];
        while (temp) {
            printf("ID: %d | %s by %s | Available: %d/%d\n",
                   temp->id, temp->title, temp->author,
                   temp->availableCopies, temp->totalCopies);
            temp = temp->next;
        }
    }
}


int main() {
    struct Library lib = {0};
    int choice, id, copies, memberId, bookId;
    char title[100], author[100], name[100];

    // --- Pre-load the requested books ---
    printf("--- Pre-loading Initial Books ---\n");
    addBook(&lib, 404, "Let Us C", "Yashavant Kanetkar", 5);
    addBook(&lib, 505, "Python Programming", "John Zelle", 10);
    // ------------------------------------

    while (1) {
        printf("\n===== Library Management System (Hash Table) =====\n");
        printf("1. Add Book\n2. Add Member\n3. Search Book by Title\n4. Issue Book\n5. Return Book\n6. Display All Books\n7. Exit\n");
        printf("Enter choice: ");
        
        // Check for valid integer input
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            // Clear the input buffer
            while (getchar() != '\n');
            continue;
        }
        
        getchar(); // Consume the newline left by scanf

        switch (choice) {
            case 1:
                printf("Enter Book ID: ");
                scanf("%d", &id);
                getchar(); // Consume the newline

                printf("Enter Title: ");
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = 0;

                printf("Enter Author: ");
                fgets(author, sizeof(author), stdin);
                author[strcspn(author, "\n")] = 0;

                printf("Enter Copies: ");
                scanf("%d", &copies);
                getchar(); // Consume the newline

                addBook(&lib, id, title, author, copies);
                break;

            case 2:
                printf("Enter Member ID: ");
                scanf("%d", &id);
                getchar(); // Consume the newline

                printf("Enter Name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;

                addMember(&lib, id, name);
                break;

            case 3:
                printf("Enter Title to search: ");
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = 0;

                {
                    struct Book* b = searchBookByTitle(&lib, title);
                    if (b)
                        printf("Found: %s by %s (ID: %d, %d available)\n", b->title, b->author, b->id, b->availableCopies);
                    else
                        printf("Book not found.\n");
                }
                break;

            case 4:
                printf("Enter Member ID: ");
                scanf("%d", &memberId);
                printf("Enter Book ID: ");
                scanf("%d", &bookId);
                getchar(); // Consume the newline

                issueBook(&lib, memberId, bookId);
                break;

            case 5:
                printf("Enter Member ID: ");
                scanf("%d", &memberId);
                getchar(); // Consume the newline

                returnBook(&lib, memberId);
                break;

            case 6:
                displayBooks(&lib);
                break;

            case 7:
                printf("Exiting...\n");
                
                // *** Memory Cleanup (Good Practice) ***
                // Free all allocated book nodes
                for (int i = 0; i < BOOK_TABLE_SIZE; i++) {
                    struct Book *current = lib.bookTable[i];
                    while (current != NULL) {
                        struct Book *next = current->next;
                        free(current);
                        current = next;
                    }
                }
                // Free all allocated member nodes
                for (int i = 0; i < MEMBER_TABLE_SIZE; i++) {
                    struct Member *current = lib.memberTable[i];
                    while (current != NULL) {
                        struct Member *next = current->next;
                        free(current);
                        current = next;
                    }
                }
                // *************************************

                return 0;

            default:
                printf("Invalid choice. Please enter a number between 1 and 7.\n");
        }
    }
}