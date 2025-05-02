#include "filesystem.h"
#include <string.h>  // For strcmp, strncpy (not needed if you're replacing with custom functions)

#define MAX_FILENAME 32
#define MAX_FILE_SIZE 1024
#define MAX_FILES 10
#define MAX_FILE_CONTENT_LENGTH 256

typedef struct {
    char filename[100];
    char content[MAX_FILE_CONTENT_LENGTH];
} File;

static File file_system[MAX_FILES];
static int file_count = 0;

typedef struct {
    char name[MAX_FILENAME];
    int size;
    int used;
    char data[MAX_FILE_SIZE];
} Inode;

typedef struct {
    Inode inodes[MAX_FILES];
    int file_count;
} SuperBlock;

SuperBlock fs;

// Very basic string functions
int str_eq(const char *a, const char *b) {
    while (*a && *b && (*a == *b)) {
        a++; b++;
    }
    return (*a == '\0' && *b == '\0');
}

void str_copy(char *dest, const char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void mem_clear(char *buf, int len) {
    for (int i = 0; i < len; i++) {
        buf[i] = 0;
    }
}

// Init
void fs_init() {
    fs.file_count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        fs.inodes[i].used = 0;
    }
}

// Create file
int fs_create_file(const char *name) {
    if (fs.file_count >= MAX_FILES)
        return -1;

    for (int i = 0; i < MAX_FILES; i++) {
        if (!fs.inodes[i].used) {
            fs.inodes[i].used = 1;
            str_copy(fs.inodes[i].name, name);
            fs.inodes[i].size = 0;
            mem_clear(fs.inodes[i].data, MAX_FILE_SIZE);
            fs.file_count++;
            return 0;
        }
    }
    return -1;
}

// List file names (returns pointer array)
char **fs_list_file_names(int *count) {
    static char *names[MAX_FILES];
    int idx = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.inodes[i].used) {
            names[idx++] = fs.inodes[i].name;
        }
    }
    *count = idx;
    return names;
}

// Read file (returns pointer to file content or NULL)
char *fs_read_file(const char *name) {
    for (int i = 0; i < file_count; i++) {
        if (str_eq(file_system[i].filename, name)) {
            print("Filename: ");
            print(file_system[i].filename);
            print("\nContent: ");
            print(file_system[i].content);
            print("\n");
            return file_system[i].content;
        }
    }
    print("Error: File not found.\n");
    return 0;
}


// Function to write content to a file
void fs_write_file(const char* filename, const char* content) {
    // Check if the file already exists
    for (int i = 0; i < file_count; i++) {
        if (str_eq(file_system[i].filename, filename)) {  // Replaced strcmp with str_eq
            // File found, update its content
            str_copy(file_system[i].content, content);  // Replaced strncpy with str_copy
            // If you're using print for debugging
            print("File content updated successfully.\n");
            return;
        }
    }

    // If the file doesn't exist and there's space for a new file
    if (file_count < MAX_FILES) {
        // Create a new file and add it to the file system
        str_copy(file_system[file_count].filename, filename);  // Replaced strncpy with str_copy
        str_copy(file_system[file_count].content, content);  // Replaced strncpy with str_copy
        file_count++;
        // If you're using print for debugging
        print("File created and content written successfully.\n");
    } else {
        // Error if the file system is full
        print("Error: File system is full.\n");
    }
}
