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
#define PATIENT_FILE "patients.txt"

// Patient record structure
typedef struct {
    int id;
    char patientName[STR_LEN];
    char patientAddress[STR_LEN];
    char disease[STR_LEN];
    char date[12]; // dd/mm/yyyy
} PatientRecord;

// Linked list node
typedef struct PatientNode {
    PatientRecord data;
    struct PatientNode *next;
} PatientNode;

PatientNode *patients_head = NULL;

// Utility: remove '\n' from fgets
void remove_newline(char *str) {
    str[strcspn(str, "\n")] = '\0';
}

// Utility: get current date
void get_current_date(char *dateStr) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(dateStr, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

// Load patient data from text file
void load_patients_from_file() {
    FILE *fp = fopen(PATIENT_FILE, "r");
    if (!fp) return;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        PatientRecord rec;
        char *token = strtok(line, ";");
        if (!token) continue;
        rec.id = atoi(token);

        token = strtok(NULL, ";"); if(token) strcpy(rec.patientName, token);
        token = strtok(NULL, ";"); if(token) strcpy(rec.patientAddress, token);
        token = strtok(NULL, ";"); if(token) strcpy(rec.disease, token);
        token = strtok(NULL, ";\n"); if(token) strcpy(rec.date, token);

        PatientNode *node = malloc(sizeof(PatientNode));
        node->data = rec;
        node->next = patients_head;
        patients_head = node;
    }
    fclose(fp);
}

// Save linked list to text file
void save_patients_to_file() {
    FILE *fp = fopen(PATIENT_FILE, "w");
    if (!fp) {
        perror("Error saving patient data");
        return;
    }
    PatientNode *cur = patients_head;
    while (cur) {
        fprintf(fp, "%d;%s;%s;%s;%s\n",
                cur->data.id,
                cur->data.patientName,
                cur->data.patientAddress,
                cur->data.disease,
                cur->data.date);
        cur = cur->next;
    }
    fclose(fp);
}

// Find a patient by ID
PatientNode *find_patient_by_id(int id) {
    PatientNode *cur = patients_head;
    while (cur) {
        if (cur->data.id == id) return cur;
        cur = cur->next;
    }
    return NULL;
}

// Admit new patient
void admit_patient() {
    PatientRecord rec;
    CLEAR_SCREEN();
    printf("<== Admit Patient ==>\n\n");

    printf("Enter Patient ID: ");
    scanf("%d", &rec.id);
    getchar(); // clear leftover '\n'

    if (find_patient_by_id(rec.id)) {
        printf("  Patient with ID %d already exists.\n", rec.id);
        return;
    }

    printf("Enter Patient Name: ");
    fgets(rec.patientName, STR_LEN, stdin);
    remove_newline(rec.patientName);

    printf("Enter Patient Address: ");
    fgets(rec.patientAddress, STR_LEN, stdin);
    remove_newline(rec.patientAddress);

    printf("Enter Disease: ");
    fgets(rec.disease, STR_LEN, stdin);
    remove_newline(rec.disease);

    get_current_date(rec.date);

    PatientNode *node = malloc(sizeof(PatientNode));
    node->data = rec;
    node->next = patients_head;
    patients_head = node;

    save_patients_to_file();
    printf("\n Patient admitted successfully on %s.\n", rec.date);
}

// Display all patients
void patient_list() {
    CLEAR_SCREEN();
    printf("<== Patient List ==>\n\n");
    printf("%-8s %-25s %-25s %-20s %-12s\n", "ID", "Name", "Address", "Disease", "Date");
    printf("-------------------------------------------------------------------------------\n");

    if (!patients_head) {
        printf("No patient records found.\n");
        return;
    }

    PatientNode *cur = patients_head;
    while (cur) {
        printf("%-8d %-25s %-25s %-20s %-12s\n",
               cur->data.id,
               cur->data.patientName,
               cur->data.patientAddress,
               cur->data.disease,
               cur->data.date);
        cur = cur->next;
    }
}

// Search patient by ID
void search_patient() {
    CLEAR_SCREEN();
    printf("<== Search Patient ==>\n\n");

    int id;
    printf("Enter Patient ID: ");
    scanf("%d", &id);
    getchar();

    PatientNode *node = find_patient_by_id(id);
    if (!node) {
        printf("\n Patient not found.\n");
        return;
    }

    printf("\n Patient Found:\n");
    printf("ID       : %d\n", node->data.id);
    printf("Name     : %s\n", node->data.patientName);
    printf("Address  : %s\n", node->data.patientAddress);
    printf("Disease  : %s\n", node->data.disease);
    printf("Admit Date: %s\n", node->data.date);
}

// Update patient info
void update_patient() {
    CLEAR_SCREEN();
    printf("<== Update Patient ==>\n\n");

    int id;
    printf("Enter Patient ID to update: ");
    scanf("%d", &id);
    getchar();

    PatientNode *node = find_patient_by_id(id);
    if (!node) {
        printf("\n Patient not found.\n");
        return;
    }

    printf("\nCurrent Name: %s\n", node->data.patientName);
    printf("Enter New Name (press Enter to skip): ");
    char temp[STR_LEN];
    fgets(temp, STR_LEN, stdin);
    remove_newline(temp);
    if (strlen(temp) > 0) strcpy(node->data.patientName, temp);

    printf("\nCurrent Address: %s\n", node->data.patientAddress);
    printf("Enter New Address (press Enter to skip): ");
    fgets(temp, STR_LEN, stdin);
    remove_newline(temp);
    if (strlen(temp) > 0) strcpy(node->data.patientAddress, temp);

    printf("\nCurrent Disease: %s\n", node->data.disease);
    printf("Enter New Disease (press Enter to skip): ");
    fgets(temp, STR_LEN, stdin);
    remove_newline(temp);
    if (strlen(temp) > 0) strcpy(node->data.disease, temp);

    save_patients_to_file();
    printf("\n Patient updated successfully.\n");
}

// Discharge patient (delete)
void discharge_patient() {
    CLEAR_SCREEN();
    printf("<== Discharge Patient ==>\n\n");

    int id;
    printf("Enter Patient ID to discharge: ");
    scanf("%d", &id);
    getchar();

    PatientNode *cur = patients_head, *prev = NULL;
    while (cur && cur->data.id != id) {
        prev = cur;
        cur = cur->next;
    }

    if (!cur) {
        printf("\n Patient not found.\n");
        return;
    }

    if (prev == NULL) {
        patients_head = cur->next;
    } else {
        prev->next = cur->next;
    }
    free(cur);
    save_patients_to_file();

    printf("\n Patient discharged successfully.\n");
}

// Free memory
void free_all_patients() {
    PatientNode *cur = patients_head;
    while (cur) {
        PatientNode *temp = cur;
        cur = cur->next;
        free(temp);
    }
}

// Patient Menu
void patient_menu() {
    int choice;
    while (1) {
        CLEAR_SCREEN();
        printf("<== Hospital Management System : Patient Module ==>\n");
        printf("1. Admit Patient\n");
        printf("2. Show Patient List\n");
        printf("3. Search Patient\n");
        printf("4. Update Patient\n");
        printf("5. Discharge Patient\n");
        printf("0. Exit\n\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: admit_patient(); break;
            case 2: patient_list(); break;
            case 3: search_patient(); break;
            case 4: update_patient(); break;
            case 5: discharge_patient(); break;
            case 0: return;
            default: printf(" Invalid choice.\n");
        }

        printf("\nPress Enter to continue...");
        getchar();
    }
}

// Main
int main() {
    load_patients_from_file();
    patient_menu();
    free_all_patients();
    printf("\n Exiting Patient Module...\n");
    return 0;
}
