/***********HAMZA AWASHRA
 **********1201619
 **********SEC 3
 **********PROJECT 3 (HASH TABLES)
 ********************/
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

#define MAX_COURSE_LINE 512
#define MAX_TOPICS_LINE 256
#define MAX_COURSE_NAME 50
#define MAX_COURSE_CODE 30
#define MAX_DEPARTMENT_NAME 50

struct course {
    char courseName[MAX_COURSE_NAME];
    int creditHours;
    char courseCode[MAX_COURSE_CODE];
    char department[MAX_DEPARTMENT_NAME];
    struct node_str *topics_list;
};

struct node_str {
    char elem[MAX_COURSE_CODE];
    struct node_str *next;
};

typedef struct node_str *topicNode;

void insertTopicLast(struct node_str *head, char topic[]);

struct hashBlock {
    struct course data;
    int status;
};

typedef struct hashBlock *hashPtr;

typedef hashPtr *HashTable;

HashTable hashTable_DH; // hash table (double hashing)

/* (double hashing) table size and count (number of occupied indices) */
unsigned tableSize_DH; // size for double hashing
unsigned count_DH = 0; // count for double hashing
unsigned noOfCollisions_DH = 0;
/* (open addressing) table size and count(number of occupied indices) */
unsigned tableSize_OA;
unsigned count_OA = 0;
unsigned noOfCollisions_OA = 0;

HashTable hashTable_OA;

unsigned R = 0;

char *trimmed(char *str);

void readFileContent();

unsigned findPrimeBeforeX(unsigned x);

unsigned doubleHashFunc(unsigned index, char *key, unsigned tableSize, HashTable *hashTable);

void init_hashTable(HashTable *hashPtr, unsigned tableSize);

unsigned findNextPrimeAfterValue(unsigned value);

void insertToHashDH(HashTable *hashPtr, unsigned tableSize, struct course course);

void printHashTable(HashTable *hashPtr, unsigned tableSize);

void printCourseTopics(topicNode topics);

void insertToHash_OA(HashTable *hashPtr, unsigned tableSize, struct course course);

void insertNewFromUser();

unsigned searchInDH(char *key, unsigned tableSize);

void searchInHashTable();

void copyDataToHashTable(HashTable *hashPtr, struct course course, unsigned index);

void rehash_DH(HashTable *hashPtr, unsigned previousSize);

unsigned searchInOA(char *key);

void rehash_OA(HashTable *hashPtr, unsigned previousSize);

void removeLineAtEnd(char string[50]);

void deleteFromHashTable();

void saveToFile();

void printCourseInfo(struct course course1);

int main() {

    FILE *fptr = fopen("courses.txt", "r");

    if (!fptr) {
        printf("COULD NOT FIND THE FILE\n\n");
    }


    unsigned x = 0;
    char arr[512];
    while (fgets(arr, 512, fptr) != NULL)
        ++x;
    fclose(fptr);

//    printf("\n%d\n", x);
    //------------------------------------------------------------------------------------------------------------------
/*  above I calculated the number of courses in the file, to determine the size of the hashTable. */
    // calling a method to find the next prime after 2*x where x is number of courses we have in the file.
    tableSize_DH = findNextPrimeAfterValue(2 * x);
    init_hashTable(&hashTable_DH,
                   tableSize_DH); // allocating memory for the hashTable and initializing indices status to 0 (unused)

    tableSize_OA = tableSize_DH;
    init_hashTable(&hashTable_OA, tableSize_OA); // locate memory and initialize status to 0 for open hashing table.

    printf("%u\n\n", tableSize_DH);

     R = findPrimeBeforeX(tableSize_DH);


    readFileContent(); // reading the file and inserting the elements in the hashTable.
    while (true) {
        printf("1. Print hashed tables (including empty spots).\n"
               "2. Print out table size and the load factor.\n"
               "3. Print out the used hash functions.\n"
               "4. Insert a new record to hash table (insertion will be done on both hash tables).\n"
               "5. Search for a specific word (specify which table to search in).\n"
               "6. Delete a specific record (from both tables).\n"
               "7. Compare between the two methods in terms of number of collisions occurred.\n"
               "8. Save hash table back to a file named saved_courses.txt (of the double hashing)\n");


        printf("Enter your choice number: ");
        int choice;
        scanf("%d", &choice);


        switch (choice) {
            case 1:

                printf("\n\n\nDouble hashing table data:\n\n ");
                printf("\n%-35s%-15s%-20s%s\n", "COURSE NAME", "CREDIT HOURS", "COURSE CODE", "DEPARTMENT");
                printHashTable(&hashTable_DH, tableSize_DH);
                printf("\n_______________________________________________________________________________________________\n\n\n");
                printf("Open addressing table data:\n\n ");
                printf("%-35s%-15s%-20s%s\n", "COURSE NAME", "CREDIT HOURS", "COURSE CODE", "DEPARTMENT");
                printHashTable(&hashTable_OA, tableSize_OA);
                printf("\n_______________________________________________________________________________________________\n\n\n");


                break;


            case 2:
                printf("\n\nTable size is %u, and the load factor is %lf (double hashing)\n", tableSize_DH,
                       ((double) count_DH / tableSize_DH));
                printf("Table size is %u, and the load factor is %lf (open addressing)\n", tableSize_OA,
                       ((double) count_OA / tableSize_OA));
                printf("\n______________________________________________________________________________________________\n\n");

                break;


            case 3:
                printf("\n\n\nThe used function in double hashing: %s \n",
                       "h(x) = h1(x) + i * h2(x) where h1(x) = val = 0; val = ((val << 5) + char[i]) from i = 0 to len(string) - 1"
                       "and then, it takes the remainder of this mod tableSize_DH.\n"
                       "h2(X) = R - x % R where R is the first prime number less than the size of the table.\n\n");
                printf("\n\n-------------------------------------------------------------------------------------------\n\n");

                printf("The used function in open addressing (quadratic): h(x) = x mod tableSize + i*i, where x is the sum of the "
                       "ascii characters of the string.\n\n ");
                printf("\n\n-----------------------------------------------------------------------------------------------\n\n");

                break;

            case 4:
                insertNewFromUser();
                break;

            case 5:
                searchInHashTable();
                break;

            case 6:
                deleteFromHashTable();
                break;

            case 7:
                printf("\tNumber of collisions in double hashing is %d\n\tNumber of collisions in open addressing is %d\n\n",
                       noOfCollisions_DH, noOfCollisions_OA);
                break;

            case 8:
                saveToFile();

            default:
                printf("\ninvalid choice\n");
                getchar();
                break;


            case 0:
                exit(0);

        }

    }

}

void readFileContent() {

    FILE *infile = fopen("courses.txt", "r");

    if (!infile)
        printf("Couldn't open the file.");

    char line[MAX_COURSE_LINE];

    while (fgets(line, MAX_COURSE_LINE, infile) != NULL) {

        struct course course;
        course.topics_list = (struct node_str *) malloc(sizeof(struct node_str));
        course.topics_list->next = NULL;

        char topicsLine[MAX_TOPICS_LINE];

        sscanf(line, "%[^:]:%d#%[^#]#%[^/]/%[^\n]", course.courseName, &course.creditHours, course.courseCode,
               course.department, topicsLine);

        char *token;
        token = strtok(topicsLine, ",");
        insertTopicLast(course.topics_list, trimmed(token));

        while (token && *token) {
            token = strtok(NULL, ",");
            if (token && *token) {
                insertTopicLast(course.topics_list, trimmed(token));

            }
        }
        insertToHashDH(&hashTable_DH, tableSize_DH, course);
        insertToHash_OA(&hashTable_OA, tableSize_OA, course);
    }
    fclose(infile);
}

char *trimmed(char *str) {

    while (isspace((unsigned char) *str)) {
        str++;
    }
    if (*str == 0)
        return str;

    char *last;
    last = str + strlen(str) - 1;
    while ((last > str) && (isspace((unsigned char) *last))) {
        last--;
    }

    last[1] = '\0';

    /*return new trimmed string */
    return str;
}
// h1 in the equation h(x) = h1 + i*h2 (DOUBLE HASHING).
unsigned h1(char *key, unsigned tableSize) {

    int hashValue = 0;

    char *ptr = key;

    while (*ptr != '\0') {
        hashValue = (hashValue << 5) + *ptr++;
    }

    return (hashValue % tableSize);
}
// second hash function for double hashing
unsigned h2(char *key) {

    int x = 0;

    char *ptr = key;
    while (*ptr != '\0')
        x += *(ptr++);


    return (R - x % R);
}
// find the next empty spot for the course in the array
unsigned doubleHashFunc(unsigned index, char *key, unsigned tableSize, HashTable *hashTable) {
    int i = 0;
    // If the passed index already contains the key, return the index (used in search)

    unsigned nextIndex = (index + i * h2(key)) % tableSize;
    // initially, nextIndex will be equal to index. It will be updated if the index is already taken.
    while ((*hashTable)[nextIndex]->status == 1 && strcmp((*hashTable)[nextIndex]->data.courseName, key) != 0 ) {
        noOfCollisions_DH++; // collision occurred as the index is already taken.
        ++i;
        nextIndex = (index + i * h2(key)) % tableSize;
    }
    return nextIndex;
}

unsigned findPrimeBeforeX(unsigned x) {

    if (x == 2) // prime
        return x;

    if (x < 2) // not prime
        return -1;

    // check if x is prime, if yes return it, else, decrease x and call the function on it until we find a prime number
    for (int i = 2; i < sqrt(x) + 2; i++) {
        if (x % i == 0)
            return findPrimeBeforeX(--x); // recursion
    }

    return x;
}

void insertToHashDH(HashTable *hashPtr, unsigned tableSize, struct course course) {
    // find the first empty spot (using two functions to find the index)
    unsigned index = h1(course.courseName, tableSize);
    index = doubleHashFunc(index, course.courseName, tableSize, hashPtr);


    copyDataToHashTable(&hashTable_DH, course, index);
    (*hashPtr)[index]->status = 1;

    count_DH++;

    // check if table requires rehashing
    if (count_DH > tableSize / 2)
        rehash_DH(hashPtr, tableSize);

}
// copy data from struct course to the table's passed index
void copyDataToHashTable(HashTable *hashPtr, struct course course, unsigned index) {

    strcpy((*hashPtr)[index]->data.courseCode, course.courseCode);
    strcpy((*hashPtr)[index]->data.department, course.department);
    strcpy((*hashPtr)[index]->data.courseName, course.courseName);
    (*hashPtr)[index]->data.creditHours = course.creditHours;
    (*hashPtr)[index]->data.topics_list = course.topics_list;

}

// function to allocate memory for the table and initializing indices in it with status = 0 (unused)
void init_hashTable(HashTable *hashPtr, unsigned tableSize) {

    *hashPtr = (HashTable) malloc(sizeof(hashPtr) * tableSize);

    for (int i = 0; i < tableSize; i++) {
        (*hashPtr)[i] = malloc(sizeof(struct hashBlock));
        (*hashPtr)[i]->status = 0;
    }
}


// we call this function to find next prime greater than or equal to 'value'
unsigned findNextPrimeAfterValue(unsigned value) {

    if (value == 2)
        return 2;

    if (value < 2)
        return -1;

    for (int i = 2; i < sqrt(value) + 1; i++) {

        if (value % i == 0)
            return findNextPrimeAfterValue(++value);
    }
    return value;
}

// function to print the hashTable.
void printHashTable(HashTable *hashPtr, unsigned tableSize) {

    for (int i = 0; i < tableSize; i++) {
        // used, print its data
        if ((*hashPtr)[i]->status == 1)
            printCourseInfo((*hashPtr)[i]->data);
        // unused, print empty
        else if ((*hashPtr)[i]->status == 0)
            printf("\n<empty>");
        // deleted
        else if ((*hashPtr)[i]->status == -1)
            printf("\n<deleted>");
        // else (up-normal behaviour)
        else
            printf("\n<else>");
    }
}

// function to print the topics for a course.
void printCourseTopics(topicNode topics) {

    if (topics == NULL) {
        printf("NULL pointer elem");
        return;
    }

    topicNode temp = topics->next;

    while (temp != NULL) {

        if (temp->next == NULL) {
            printf("%s\n", temp->elem);
            return;
        }

        printf("%s, ", temp->elem);
        temp = temp->next;
    }
}

void insertNewFromUser() {

    getchar();
    struct course course;
    course.topics_list = (topicNode) malloc(sizeof(struct course));
    course.topics_list->next = NULL;

    printf("Enter the course information: ");
    printf("Course name, credit hours, course code, department/topic1, topic2...,topicN\n\n");
    // get the whole line from the user at one, then split it using sscanf and strtok functions

    char input[MAX_COURSE_LINE];// NAME, 3, CODE, DEP/Topic1, Topic2, Topic3
    fgets(input, MAX_COURSE_LINE, stdin);
    char *topics_line1;
    topics_line1 = strtok(input, "/");
    free(topics_line1);
    topics_line1 = strtok(NULL, "\n");

    char topics_line[MAX_TOPICS_LINE];
    strcpy(topics_line, trimmed(topics_line1));

    char *topic_i = strtok(topics_line, ",");
    insertTopicLast(course.topics_list, trimmed(topic_i));

    while (topic_i && *topic_i) {
        topic_i = strtok(NULL, ",");

        if (topic_i && *topic_i)
            insertTopicLast(course.topics_list, trimmed(topic_i));
    }

    sscanf(input, "%[^,], %d, %[^,], %[^/]/", course.courseName, &course.creditHours, course.courseCode,
           course.department);

    // print the course after inserting it (to make sure it was stored properly)
    printf("\n\n%-35s%-15d%-20s%s\n\n", course.courseName, course.creditHours, course.courseCode, course.department);

    printCourseTopics(course.topics_list);
    printf("\n\n");
    // insert the course onto both hash tables
    insertToHashDH(&hashTable_DH, tableSize_DH, course);
    insertToHash_OA(&hashTable_OA, tableSize_OA, course);

}
// holder function for searching in the table
void searchInHashTable() {

    printf("\nEnter the name of the course you want to search for:  ");
    char courseName[MAX_COURSE_NAME];
    getchar();
    fgets(courseName, MAX_COURSE_NAME, stdin);
    courseName[strlen(courseName) - 1] = '\0';

    printf("\nEnter which record you want to search in: ");

    printf("1. Double hashing table\n"
           "2. open addressing\n");

    int choice;

    scanf("%d", &choice);

    // search in double hashing
    if (choice == 1) {

        unsigned index = searchInDH(courseName, tableSize_DH);
        // not found
        if (index == -1)
            printf("\nNot found in table\n");

        else { // found
            printf("\nfound at index %d\n", index);
            printf("\n%-35s%-15s%-20s%s\n", "COURSE NAME", "CREDIT HOURS", "COURSE CODE", "DEPARTMENT");
            printCourseInfo(hashTable_DH[index]->data);
            printf("\n\nTopics: ");
            printCourseTopics(hashTable_DH[index]->data.topics_list);
            printf("\n\n");
        }
    } else if (choice == 2) {

        unsigned index = searchInOA(courseName);

        if (index == -1)
            printf("\nNot found in table\n");

        else {
            printf("\nfound at index %d\n", index);
            printf("\n%-35s%-15s%-20s%s\n", "COURSE NAME", "CREDIT HOURS", "COURSE CODE", "DEPARTMENT");
            printCourseInfo(hashTable_OA[index]->data);
            printf("\n\nTopics: ");
            printCourseTopics(hashTable_OA[index]->data.topics_list);
            printf("\n\n");
        }
    }
}
// method to print course info without topics
void printCourseInfo(struct course course) {

    printf("\n%-35s%-15d%-20s%s", course.courseName, course.creditHours, course.courseCode, course.department);
}
// sum of ascii chars
int sumOfStringChars(char *chars) {

    char *ptr = chars;

    int sum = 0;
    while (ptr && *ptr)
        sum += (*ptr++);

    return sum;
}
// hash function for open addressing
unsigned hash_OA(char *key, unsigned tableSize) {

    int val = sumOfStringChars(key);
    return val % tableSize;
}

unsigned quadraticHashFunc(HashTable *hashPtr,char* key, unsigned tableSize, unsigned index) {

    int i = 0;
    unsigned nextIndex = index;

    while ((*hashPtr)[nextIndex]->status == 1 && strcmp((*hashPtr)[nextIndex]->data.courseName, key) != 0) {
        noOfCollisions_OA++; // collision occurred as the index is taken (or deleted)
        ++i;
        nextIndex = (index + i * i) % tableSize;
    }

    return nextIndex;
}

void insertToHash_OA(HashTable *hashPtr, unsigned tableSize, struct course course) {

    // get the next empty index to insert in it.
    unsigned index = hash_OA(course.courseName, tableSize);
    index = quadraticHashFunc(hashPtr,course.courseName, tableSize, index);
    // copy data to the table in the specified index
    copyDataToHashTable(&hashTable_OA, course, index);
    (*hashPtr)[index]->status = 1;
    // increment number of elements in table
    count_OA++;
    // check if we need to rehash
    if (count_OA > tableSize_OA / 2)
        rehash_OA(&hashTable_OA, tableSize);
}

unsigned searchInDH(char *key, unsigned tableSize) {

    unsigned index = h1(key, tableSize);
    index = doubleHashFunc(index, key, tableSize, &hashTable_DH);

    if (hashTable_DH[index]->status == 1 && strcmp(hashTable_DH[index]->data.courseName, key) == 0)
        return index;

    return -1; // not found
}

unsigned searchInOA(char *key) {

    unsigned index = hash_OA(key, tableSize_OA);
    index = quadraticHashFunc(&hashTable_OA, key, tableSize_OA, index);

    if (hashTable_OA[index]->status == 1 && strcmp(hashTable_OA[index]->data.courseName, key) == 0)
        return index;

    return -1; // not found
}

void rehash_DH(HashTable *hashPtr, unsigned previousSize) {

//    noOfCollisions_DH = 0;
    unsigned newSize = findNextPrimeAfterValue(2 * previousSize);

    // update R in h2(x) = R - x % R.
    R = findPrimeBeforeX(newSize);

    HashTable newHashTable;
    // create new hash table with the new size
    init_hashTable(&newHashTable, newSize);

    int count = 0;
    for (int i = 0; i < previousSize; i++) {

        if ((*hashPtr)[i]->status == 1) {
            // get the index in the new table to insert the data in it.
            unsigned index = h1((*hashPtr)[i]->data.courseName, newSize);
            index = doubleHashFunc(index, (*hashPtr)[i]->data.courseName, newSize, &newHashTable);

            // copy the data to the index in the new table.
            copyDataToHashTable(&newHashTable, (*hashPtr)[i]->data, index);
            newHashTable[index]->status = 1;
            count++;

            // free the node from the old table.
            free((*hashPtr)[i]);
        }
    }
    // make the old table point to the new table.
    (*hashPtr) = newHashTable;
    tableSize_DH = newSize; // change the global variable of the table size to be the new size.
    count_DH = count; // change the count of elements in the table to be the new count.

}

void rehash_OA(HashTable *hashPtr, unsigned previousSize) {

    // set number of collisions to zero, as we will re-hash every value in the table and get a different distribution of the data.
    noOfCollisions_OA = 0;
    // the new size of the hash table after rehashing, First prime after 2 times the old one.
    unsigned newSize = findNextPrimeAfterValue(2 * previousSize);

    // declare the new hash table
    HashTable newHashTable;

    // initialize the new hashtable.
    init_hashTable(&newHashTable, newSize);

    // number of elements.
    int count = 0;
    // traverse on the old table to rehash every index with status 1
    for (int i = 0; i < previousSize; i++) {

        if ((*hashPtr)[i]->status == 1) {
            // get the index in the new table. (we sent the new size to the func.)
            unsigned index = hash_OA((*hashPtr)[i]->data.courseName, newSize);
            index = quadraticHashFunc(&newHashTable,(*hashPtr)[i]->data.courseName, newSize, index );

            // copy the course's data onto the index we got from the hash function.
            copyDataToHashTable(&newHashTable, (*hashPtr)[i]->data, index);
            newHashTable[index]->status = 1;
            count++; // increase number of elements.


            // free the nodes from the old table.
            free((*hashPtr)[i]);
        }
    }
    // make the old table point to the new table.
    (*hashPtr) = newHashTable;
    tableSize_OA = newSize; // change the global variable of the tableSize to be newSize after rehashing
    count_OA = count; // change the global variable of the number of elements to be the count we got (number of indices with status 1)
}

void deleteFromHashTable() {

    getchar();
    printf("Enter the name of the course that you want to delete:");

    char courseName[MAX_COURSE_NAME];
    fgets(courseName, MAX_COURSE_NAME, stdin);
    removeLineAtEnd(courseName);

    // [index] get the index from the hash function
    unsigned index = h1(courseName, tableSize_DH);
    index = doubleHashFunc(index, courseName, tableSize_DH, &hashTable_DH);
    // [index] get the index from the hash function
    unsigned index_OA  = hash_OA(courseName, tableSize_OA);
    index_OA = quadraticHashFunc(&hashTable_OA,courseName, tableSize_OA, index_OA);

    // check if the course in the index is the course we want to delete (compare names)
    if (hashTable_DH[index]->status == 1 && strcmp(hashTable_DH[index]->data.courseName, courseName) == 0) {
        hashTable_DH[index]->status = -1;
        printf("\nThe course was found in double hash table at index %d and got deleted!\n", index);
        free(hashTable_DH[index]);
    } else
        printf("\nThe course is not found in double hashing table.\n"); // not found


    if (hashTable_OA[index_OA]->status == 1 && strcmp(hashTable_OA[index_OA]->data.courseName, courseName) == 0) {
        hashTable_OA[index]->status = -1;
        printf("\nThe course was found in open addressing table at index %d and got deleted!\n\n", index_OA);
        free(hashTable_OA[index_OA]);
    } else
        printf("\nThe course is not found in open addressing table.\n\n"); // not found
}

void removeLineAtEnd(char string[]) {
    char *ptr = string;
    // find \n or \0
    while (*ptr != '\n' && *ptr != '\0')
        ptr++;

    // change \n to \0 if it was found, else do nothing to the string.
    if (*ptr == '\n')
        *ptr = '\0';
}

void saveToFile() {

    FILE *ptrOut = fopen("saved_courses.txt", "w");

    if (!ptrOut)
        printf("output file not found!");

    printf("Thanks, good bye!");

    for (int i = 0; i < tableSize_DH; i++) {

        if (hashTable_DH[i]->status == 1) {
            fprintf(ptrOut, "%s:%d#%s#%s/", hashTable_DH[i]->data.courseName, hashTable_DH[i]->data.creditHours,
                    hashTable_DH[i]->data.courseCode, hashTable_DH[i]->data.department);

            topicNode temp = hashTable_DH[i]->data.topics_list->next;

            while (temp != NULL) {
                if (temp->next != NULL)
                    fprintf(ptrOut, "%s, ", temp->elem);
                else
                    fprintf(ptrOut, "%s\n", temp->elem);

                temp = temp->next;
            }
        }
    }

    fclose(ptrOut);
}

void insertTopicLast(struct node_str *head, char topic[]) {

    if (head == NULL) {
        printf("NULL head");
        return;
    }

    topicNode t = (topicNode) malloc(sizeof(struct node_str));
    strcpy(t->elem, topic);
    t->next = NULL;

    topicNode temp = head;
    // traverse to the end of the linked list (last node)
    while (temp->next)
        temp = temp->next;
    // insert after the last node (to maintain the original order of the topics).
    temp->next = t;
}
