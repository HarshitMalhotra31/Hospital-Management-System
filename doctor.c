#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
  #define CLEAR_SCREEN() system("cls")
#else
  #define CLEAR_SCREEN() system("clear")
#endif

#define STR_LEN 64
#define DOCTOR_FILE "doctors.txt"  // Text file to store doctor records

// Structure to store doctor information
typedef struct {
    int id;                        // Unique doctor ID
    char name[STR_LEN];             // Doctor's name
    char address[STR_LEN];          // Doctor's address
    char specialization[STR_LEN];   // Specialization
    char date[12];                  // Date added (dd/mm/yyyy)
} DoctorRecord;

// Node for linked list
typedef struct DoctorNode {
    DoctorRecord data;
    struct DoctorNode *next;
} DoctorNode;

DoctorNode *doctors_head = NULL;

// Remove trailing newline character
void remove_newline(char *str) {
    str[strcspn(str, "\n")] = '\0';
}

// Get current system date as string
void get_current_date(char *dateStr) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(dateStr, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

// Load doctors from file into linked list
void load_doctors_from_file() {
    FILE *fp = fopen(DOCTOR_FILE, "r");
    if (!fp) return;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        DoctorRecord rec;
        char *token = strtok(line, ";");
        if (!token) continue;
        rec.id = atoi(token);

        token = strtok(NULL, ";"); if(token) strcpy(rec.name, token);
        token = strtok(NULL, ";"); if(token) strcpy(rec.address, token);
        token = strtok(NULL, ";"); if(token) strcpy(rec.specialization, token);
        token = strtok(NULL, ";\n"); if(token) strcpy(rec.date, token);

        DoctorNode *node = malloc(sizeof(DoctorNode));
        node->data = rec;
        node->next = doctors_head;
        doctors_head = node;
    }
    fclose(fp);
}

// Save linked list to file
void save_doctors_to_file() {
    FILE *fp = fopen(DOCTOR_FILE, "w");
    if (!fp) { perror("Error saving doctor data"); return; }

    DoctorNode *cur = doctors_head;
    while (cur) {
        fprintf(fp, "%d;%s;%s;%s;%s\n",
                cur->data.id,
                cur->data.name,
                cur->data.address,
                cur->data.specialization,
                cur->data.date);
        cur = cur->next;
    }
    fclose(fp);
}

// Find doctor by ID
DoctorNode *find_doctor_by_id(int id) {
    DoctorNode *cur = doctors_head;
    while (cur) {
        if (cur->data.id == id) return cur;
        cur = cur->next;
    }
    return NULL;
}

// Add new doctor
void add_doctor() {
    DoctorRecord rec;
    char buffer[STR_LEN];
    CLEAR_SCREEN();
    printf("<== Add Doctor ==>\n\n");

    // Read doctor ID as string and convert to int
    printf("Enter Doctor ID: ");
    fgets(buffer, STR_LEN, stdin);
    rec.id = atoi(buffer);

    if (find_doctor_by_id(rec.id)) {
        printf("  Doctor with ID %d already exists.\n", rec.id);
        return;
    }

    // Read name
    printf("Enter Doctor Name: ");
    fgets(rec.name, STR_LEN, stdin); remove_newline(rec.name);

    // Read address
    printf("Enter Doctor Address: ");
    fgets(rec.address, STR_LEN, stdin); remove_newline(rec.address);

    // Read specialization
    printf("Enter Specialization: ");
    fgets(rec.specialization, STR_LEN, stdin); remove_newline(rec.specialization);

    // Set current date
    get_current_date(rec.date);

    // Add to linked list
    DoctorNode *node = malloc(sizeof(DoctorNode));
    node->data = rec;
    node->next = doctors_head;
    doctors_head = node;

    save_doctors_to_file();
    printf("\n Doctor added successfully on %s.\n", rec.date);
}

// Display all doctors
void doctor_list() {
    CLEAR_SCREEN();
    printf("<== Doctor List ==>\n\n");
    printf("%-8s %-25s %-25s %-25s %-12s\n", "ID", "Name", "Address", "Specialization", "Date");
    printf("-----------------------------------------------------------------------------------------------\n");

    if (!doctors_head) { printf("No doctor records found.\n"); return; }

    DoctorNode *cur = doctors_head;
    while (cur) {
        printf("%-8d %-25s %-25s %-25s %-12s\n",
               cur->data.id,
               cur->data.name,
               cur->data.address,
               cur->data.specialization,
               cur->data.date);
        cur = cur->next;
    }
}

// Search doctor by ID
void search_doctor() {
    CLEAR_SCREEN();
    printf("<== Search Doctor ==>\n\n");
    char buffer[STR_LEN];
    printf("Enter Doctor ID: ");
    fgets(buffer, STR_LEN, stdin);
    int id = atoi(buffer);

    DoctorNode *node = find_doctor_by_id(id);
    if (!node) { printf("\n Doctor not found.\n"); return; }

    printf("\n Doctor Found:\n");
    printf("ID            : %d\n", node->data.id);
    printf("Name          : %s\n", node->data.name);
    printf("Address       : %s\n", node->data.address);
    printf("Specialization: %s\n", node->data.specialization);
    printf("Date Added    : %s\n", node->data.date);
}

// Update doctor info
void update_doctor() {
    CLEAR_SCREEN();
    printf("<== Update Doctor ==>\n\n");
    char buffer[STR_LEN];
    printf("Enter Doctor ID to update: ");
    fgets(buffer, STR_LEN, stdin);
    int id = atoi(buffer);

    DoctorNode *node = find_doctor_by_id(id);
    if (!node) { printf("\n Doctor not found.\n"); return; }

    // Update Name
    printf("\nCurrent Name: %s\n", node->data.name);
    printf("Enter New Name (press Enter to skip): ");
    fgets(buffer, STR_LEN, stdin); remove_newline(buffer);
    if (strlen(buffer) > 0) strcpy(node->data.name, buffer);

    // Update Address
    printf("\nCurrent Address: %s\n", node->data.address);
    printf("Enter New Address (press Enter to skip): ");
    fgets(buffer, STR_LEN, stdin); remove_newline(buffer);
    if (strlen(buffer) > 0) strcpy(node->data.address, buffer);

    // Update Specialization
    printf("\nCurrent Specialization: %s\n", node->data.specialization);
    printf("Enter New Specialization (press Enter to skip): ");
    fgets(buffer, STR_LEN, stdin); remove_newline(buffer);
    if (strlen(buffer) > 0) strcpy(node->data.specialization, buffer);

    save_doctors_to_file();
    printf("\n Doctor updated successfully.\n");
}

// Delete doctor
void delete_doctor() {
    CLEAR_SCREEN();
    printf("<== Delete Doctor ==>\n\n");
    char buffer[STR_LEN];
    printf("Enter Doctor ID to delete: ");
    fgets(buffer, STR_LEN, stdin);
    int id = atoi(buffer);

    DoctorNode *cur = doctors_head, *prev = NULL;
    while (cur && cur->data.id != id) {
        prev = cur; cur = cur->next;
    }

    if (!cur) { printf("\n Doctor not found.\n"); return; }

    if (!prev) doctors_head = cur->next;
    else prev->next = cur->next;
    free(cur);
    save_doctors_to_file();

    printf("\n Doctor deleted successfully.\n");
}

// Free memory
void free_all_doctors() {
    DoctorNode *cur = doctors_head;
    while (cur) {
        DoctorNode *temp = cur;
        cur = cur->next;
        free(temp);
    }
}

// Doctor module menu
void doctor_menu() {
    char buffer[STR_LEN];
    int choice;
    while (1) {
        CLEAR_SCREEN();
        printf("<== Hospital Management System : Doctor Module ==>\n");
        printf("1. Add Doctor\n");
        printf("2. Show Doctor List\n");
        printf("3. Search Doctor\n");
        printf("4. Update Doctor\n");
        printf("5. Delete Doctor\n");
        printf("0. Exit\n\n");
        printf("Enter your choice: ");
        fgets(buffer, STR_LEN, stdin);
        choice = atoi(buffer);

        switch (choice) {
            case 1: add_doctor(); break;
            case 2: doctor_list(); break;
            case 3: search_doctor(); break;
            case 4: update_doctor(); break;
            case 5: delete_doctor(); break;
            case 0: return;
            default: printf(" Invalid choice.\n");
        }
        printf("\nPress Enter to continue..."); fgets(buffer, STR_LEN, stdin);
    }
}

// Main function
int main() {
    load_doctors_from_file();
    doctor_menu();
    free_all_doctors();
    printf("\n Exiting Doctor Module...\n");
    return 0;
}


