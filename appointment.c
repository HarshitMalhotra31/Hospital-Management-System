#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
  #define CLEAR_SCREEN() system("cls")
#else
  #define CLEAR_SCREEN() system("clear")
#endif

#define STR_LEN 64
#define APPOINT_FILE "appointments.txt"   // changed to text file
#define PATIENT_FILE "patients.txt"
#define DOCTOR_FILE "doctors.txt"

// Appointment structure
typedef struct {
    int appointmentID;
    int patientID;
    int doctorID;
    char date[12];     // dd/mm/yyyy
    char time[10];     // hh:mm
    char purpose[STR_LEN];
} AppointmentRecord;

// Linked list node
typedef struct AppointmentNode {
    AppointmentRecord data;
    struct AppointmentNode *next;
} AppointmentNode;

AppointmentNode *appointments_head = NULL;

// Patient structure for validation
typedef struct {
    int id;
    char patientName[STR_LEN];
    char patientAddress[STR_LEN];
    char disease[STR_LEN];
    char date[12];
} PatientRecord;

// Doctor structure for validation
typedef struct {
    int id;
    char name[STR_LEN];
    char address[STR_LEN];
    char specialization[STR_LEN];
    char date[12];
} DoctorRecord;

// Utility: remove '\n'
void remove_newline(char *str) {
    str[strcspn(str, "\n")] = '\0';
}

// Load appointments from text file
void load_appointments_from_file() {
    FILE *fp = fopen(APPOINT_FILE, "r");
    if (!fp) return;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        AppointmentRecord rec;
        char *token = strtok(line, ";");
        if (!token) continue;
        rec.appointmentID = atoi(token);

        token = strtok(NULL, ";"); if(token) rec.patientID = atoi(token);
        token = strtok(NULL, ";"); if(token) rec.doctorID = atoi(token);
        token = strtok(NULL, ";"); if(token) strcpy(rec.date, token);
        token = strtok(NULL, ";"); if(token) strcpy(rec.time, token);
        token = strtok(NULL, ";\n"); if(token) strcpy(rec.purpose, token);

        AppointmentNode *node = malloc(sizeof(AppointmentNode));
        node->data = rec;
        node->next = appointments_head;
        appointments_head = node;
    }
    fclose(fp);
}

// Save appointments to text file
void save_appointments_to_file() {
    FILE *fp = fopen(APPOINT_FILE, "w");
    if (!fp) { perror("Error saving appointments"); return; }

    AppointmentNode *cur = appointments_head;
    while (cur) {
        fprintf(fp, "%d;%d;%d;%s;%s;%s\n",
                cur->data.appointmentID,
                cur->data.patientID,
                cur->data.doctorID,
                cur->data.date,
                cur->data.time,
                cur->data.purpose);
        cur = cur->next;
    }
    fclose(fp);
}

// Validate patient ID
int patient_exists(int id) {
    FILE *fp = fopen(PATIENT_FILE, "r");
    if (!fp) return 0;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        PatientRecord p;
        char *token = strtok(line, ";");
        if (!token) continue;
        p.id = atoi(token);
        if (p.id == id) { fclose(fp); return 1; }
    }
    fclose(fp);
    return 0;
}

// Validate doctor ID
int doctor_exists(int id) {
    FILE *fp = fopen(DOCTOR_FILE, "r");
    if (!fp) return 0;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        DoctorRecord d;
        char *token = strtok(line, ";");
        if (!token) continue;
        d.id = atoi(token);
        if (d.id == id) { fclose(fp); return 1; }
    }
    fclose(fp);
    return 0;
}

// Find appointment by ID
AppointmentNode *find_appointment_by_id(int id) {
    AppointmentNode *cur = appointments_head;
    while (cur) {
        if (cur->data.appointmentID == id) return cur;
        cur = cur->next;
    }
    return NULL;
}

// Book appointment
void book_appointment() {
    AppointmentRecord rec;
    CLEAR_SCREEN();
    printf("<== Book Appointment ==>\n\n");

    printf("Enter Appointment ID: ");
    scanf("%d", &rec.appointmentID); getchar();
    if (find_appointment_by_id(rec.appointmentID)) {
        printf(" Appointment with ID %d already exists.\n", rec.appointmentID);
        return;
    }

    printf("Enter Patient ID: ");
    scanf("%d", &rec.patientID); getchar();
    if (!patient_exists(rec.patientID)) { printf(" Patient ID not found.\n"); return; }

    printf("Enter Doctor ID: ");
    scanf("%d", &rec.doctorID); getchar();
    if (!doctor_exists(rec.doctorID)) { printf(" Doctor ID not found.\n"); return; }

    printf("Enter Appointment Date (DD/MM/YYYY): ");
    fgets(rec.date, sizeof(rec.date), stdin); remove_newline(rec.date);

    printf("Enter Appointment Time (HH:MM): ");
    fgets(rec.time, sizeof(rec.time), stdin); remove_newline(rec.time);

    printf("Enter Purpose: ");
    fgets(rec.purpose, STR_LEN, stdin); remove_newline(rec.purpose);

    AppointmentNode *node = malloc(sizeof(AppointmentNode));
    node->data = rec;
    node->next = appointments_head;
    appointments_head = node;

    save_appointments_to_file();
    printf("\n Appointment booked successfully.\n");
}

// Show all appointments
void show_appointments() {
    CLEAR_SCREEN();
    printf("<== All Appointments ==>\n\n");
    printf("%-8s %-10s %-10s %-12s %-8s %-25s\n",
           "AppID", "PatientID", "DoctorID", "Date", "Time", "Purpose");
    printf("------------------------------------------------------------------------------------------\n");

    if (!appointments_head) { printf("No appointments found.\n"); return; }

    AppointmentNode *cur = appointments_head;
    while (cur) {
        printf("%-8d %-10d %-10d %-12s %-8s %-25s\n",
               cur->data.appointmentID,
               cur->data.patientID,
               cur->data.doctorID,
               cur->data.date,
               cur->data.time,
               cur->data.purpose);
        cur = cur->next;
    }
}

// Search appointment
void search_appointment() {
    CLEAR_SCREEN();
    printf("<== Search Appointment ==>\n\n");

    int id;
    printf("Enter Appointment ID: "); scanf("%d", &id); getchar();
    AppointmentNode *node = find_appointment_by_id(id);
    if (!node) { printf("\n Appointment not found.\n"); return; }

    printf("\nAppointment Found:\n");
    printf("Appointment ID: %d\n", node->data.appointmentID);
    printf("Patient ID    : %d\n", node->data.patientID);
    printf("Doctor ID     : %d\n", node->data.doctorID);
    printf("Date          : %s\n", node->data.date);
    printf("Time          : %s\n", node->data.time);
    printf("Purpose       : %s\n", node->data.purpose);
}

// Update appointment
void update_appointment() {
    CLEAR_SCREEN();
    printf("<== Update Appointment ==>\n\n");

    int id;
    printf("Enter Appointment ID to update: "); scanf("%d", &id); getchar();
    AppointmentNode *node = find_appointment_by_id(id);
    if (!node) { printf("\n Appointment not found.\n"); return; }

    char temp[STR_LEN];
    printf("Current Date: %s\nEnter New Date (Enter to skip): ", node->data.date);
    fgets(temp, sizeof(temp), stdin); remove_newline(temp);
    if (strlen(temp) > 0) strcpy(node->data.date, temp);

    printf("Current Time: %s\nEnter New Time (Enter to skip): ", node->data.time);
    fgets(temp, sizeof(temp), stdin); remove_newline(temp);
    if (strlen(temp) > 0) strcpy(node->data.time, temp);

    printf("Current Purpose: %s\nEnter New Purpose (Enter to skip): ", node->data.purpose);
    fgets(temp, sizeof(temp), stdin); remove_newline(temp);
    if (strlen(temp) > 0) strcpy(node->data.purpose, temp);

    save_appointments_to_file();
    printf("\n Appointment updated successfully.\n");
}

// Cancel appointment
void cancel_appointment() {
    CLEAR_SCREEN();
    printf("<== Cancel Appointment ==>\n\n");

    int id; printf("Enter Appointment ID to cancel: "); scanf("%d", &id); getchar();
    AppointmentNode *cur = appointments_head, *prev = NULL;
    while (cur && cur->data.appointmentID != id) { prev = cur; cur = cur->next; }

    if (!cur) { printf("\n Appointment not found.\n"); return; }

    if (!prev) appointments_head = cur->next;
    else prev->next = cur->next;
    free(cur);
    save_appointments_to_file();
    printf("\n Appointment canceled successfully.\n");
}

// Free memory
void free_all_appointments() {
    AppointmentNode *cur = appointments_head;
    while (cur) { AppointmentNode *temp = cur; cur = cur->next; free(temp); }
}

// Appointment menu
void appointment_menu() {
    int choice;
    while (1) {
        CLEAR_SCREEN();
        printf("<== Hospital Management System : Appointment Module ==>\n");
        printf("1. Book Appointment\n2. View All Appointments\n3. Search Appointment\n");
        printf("4. Update Appointment\n5. Cancel Appointment\n0. Exit\n\n");
        printf("Enter your choice: "); scanf("%d", &choice); getchar();

        switch(choice){
            case 1: book_appointment(); break;
            case 2: show_appointments(); break;
            case 3: search_appointment(); break;
            case 4: update_appointment(); break;
            case 5: cancel_appointment(); break;
            case 0: return;
            default: printf("Invalid choice.\n");
        }
        printf("\nPress Enter to continue..."); getchar();
    }
}

// Main
int main() {
    load_appointments_from_file();
    appointment_menu();
    free_all_appointments();
    printf("\nExiting Appointment Module...\n");
    return 0;
}
