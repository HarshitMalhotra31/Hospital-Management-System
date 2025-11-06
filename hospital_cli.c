#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef _WIN32
#define EXEC_BILLING "billing.exe"
#define EXEC_EMER "emergency.exe"
#define EXEC_DOCTOR "doctor.exe"
#define EXEC_PATIENT "patient.exe"
#define EXEC_APPT "appointment.exe"
#define OPEN_CMD(file) "start \"\" \"" file "\""
#else
#define EXEC_BILLING "./billing"
#define EXEC_EMER "./emergency"
#define EXEC_DOCTOR "./doctor"
#define EXEC_PATIENT "./patient"
#define EXEC_APPT "./appointment"
#define OPEN_CMD(file) "xdg-open \"" file "\" 2>/dev/null || open \"" file "\""
#endif

int exists(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    fclose(f);
    return 1;
}

void try_compile_binaries() {
    const char *modules[][2] = {
        {"billing.c", "billing"},
        {"emergency.c", "emergency"},
        {"doctor.c", "doctor"},
        {"patient.c", "patient"},
        {"appointment.c", "appointment"}
    };
    int n = sizeof(modules) / sizeof(modules[0]);
    for (int i = 0; i < n; i++) {
        if (!exists(modules[i][1])) {
            if (exists(modules[i][0])) {
                char cmd[256];
                snprintf(cmd, sizeof(cmd), "gcc %s -o %s", modules[i][0], modules[i][1]);
                printf("Compiling %s...\n", modules[i][0]);
                int r = system(cmd);
                if (r == 0)
                    printf("%s compiled successfully.\n", modules[i][1]);
                else
                    printf("Failed to compile %s.\n", modules[i][0]);
            }
        }
    }
}

int main_menu() {
    int ch;
    printf("=== Hospital Console GUI ===\n");
    printf("1. Open Billing Module\n");
    printf("2. Open Emergency Module\n");
    printf("3. Open Doctor Module\n");
    printf("4. Open Patient Module\n");
    printf("5. Open Appointment Module\n");
    printf("6. View bills.txt\n");
    printf("7. View emergencies.txt\n");
    printf("8. View doctors.txt\n");
    printf("9. View patients.txt\n");
    printf("10. View appointments.txt\n");
    printf("11. Try compile all modules (if sources present)\n");
    printf("0. Exit\n");
    printf("Choice: ");
    
    if (scanf("%d", &ch) != 1) {
        while (getchar() != '\n'); // clear input buffer
        return -1;
    }
    while (getchar() != '\n'); // clear newline
    return ch;
}

int main() {
    while (1) {
        int choice = main_menu();
        if (choice < 0) continue;

        switch (choice) {
            case 1:
                if (exists(EXEC_BILLING)) system(EXEC_BILLING);
                else printf("Billing module not found.\n");
                break;
            case 2:
                if (exists(EXEC_EMER)) system(EXEC_EMER);
                else printf("Emergency module not found.\n");
                break;
            case 3:
                if (exists(EXEC_DOCTOR)) system(EXEC_DOCTOR);
                else printf("Doctor module not found.\n");
                break;
            case 4:
                if (exists(EXEC_PATIENT)) system(EXEC_PATIENT);
                else printf("Patient module not found.\n");
                break;
            case 5:
                if (exists(EXEC_APPT)) system(EXEC_APPT);
                else printf("Appointment module not found.\n");
                break;
            case 6:
                system(OPEN_CMD("bills.txt"));
                break;
            case 7:
                system(OPEN_CMD("emergencies.txt"));
                break;
            case 8:
                system(OPEN_CMD("doctors.txt"));
                break;
            case 9:
                system(OPEN_CMD("patients.txt"));
                break;
            case 10:
                system(OPEN_CMD("appointments.txt"));
                break;
            case 11:
                try_compile_binaries();
                break;
            case 0:
                printf("Exiting.\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }

        printf("\nPress Enter to continue...");
        while (getchar() != '\n');
    }
    return 0;
}

