#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <limits>

#ifdef _WIN32
  #define EXEC_BILLING "billing.exe"
  #define EXEC_EMER "emergency.exe"
  #define EXEC_DOCTOR "doctor.exe"
  #define EXEC_PATIENT "patient.exe"
  #define EXEC_APPT "appointment.exe"
  #define OPEN_CMD(file) (std::string("start \"\" \"") + (file) + "\"")
#else
  #define EXEC_BILLING "./billing"
  #define EXEC_EMER "./emergency"
  #define EXEC_DOCTOR "./doctor"
  #define EXEC_PATIENT "./patient"
  #define EXEC_APPT "./appointment"
  #define OPEN_CMD(file) (std::string("xdg-open \"") + (file) + "\" 2>/dev/null || open \"" + (file) + "\"")
#endif

bool exists(const std::string &path) {
    FILE *f = fopen(path.c_str(), "r");
    if (!f) return false;
    fclose(f);
    return true;
}

void try_compile_binaries() {
    const char* modules[][2] = {
        {"billing.c", "billing"},
        {"emergency.c", "emergency"},
        {"doctor.c", "doctor"},
        {"patient.c", "patient"},
        {"appointment.c", "appointment"}
    };
    for (auto &m : modules) {
        if (!exists(m[1])) {
            if (exists(m[0])) {
                std::string cmd = std::string("gcc ") + m[0] + " -o " + m[1];
                std::cout << "Compiling " << m[0] << "...\n";
                int r = system(cmd.c_str());
                std::cout << (r==0 ? std::string(m[1]) + " compiled.\n" : "Failed to compile.\n");
            }
        }
    }
}

int main_menu() {
    std::cout << "=== Hospital Console GUI ===\n";
    std::cout << "1. Open Billing Module\n";
    std::cout << "2. Open Emergency Module\n";
    std::cout << "3. Open Doctor Module\n";
    std::cout << "4. Open Patient Module\n";
    std::cout << "5. Open Appointment Module\n";
    std::cout << "6. View bills.txt\n";
    std::cout << "7. View emergencies.txt\n";
    std::cout << "8. View doctors.txt\n";
    std::cout << "9. View patients.txt\n";
    std::cout << "10. View appointments.txt\n";
    std::cout << "11. Try compile all modules (if sources present)\n";
    std::cout << "0. Exit\n";
    std::cout << "Choice: ";
    int ch;
    if (!(std::cin >> ch)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        return -1;
    }
    std::cin.ignore(10000, '\n');
    return ch;
}

int main() {
    while (true) {
        int choice = main_menu();
        if (choice < 0) continue;

        switch (choice) {
            case 1: 
                if (exists(EXEC_BILLING)) system(EXEC_BILLING);
                else std::cout << "Billing module not found.\n";
                break;
            case 2: 
                if (exists(EXEC_EMER)) system(EXEC_EMER);
                else std::cout << "Emergency module not found.\n";
                break;
            case 3: 
                if (exists(EXEC_DOCTOR)) system(EXEC_DOCTOR);
                else std::cout << "Doctor module not found.\n";
                break;
            case 4: 
                if (exists(EXEC_PATIENT)) system(EXEC_PATIENT);
                else std::cout << "Patient module not found.\n";
                break;
            case 5: 
                if (exists(EXEC_APPT)) system(EXEC_APPT);
                else std::cout << "Appointment module not found.\n";
                break;
            case 6: system(OPEN_CMD("bills.txt").c_str()); break;
            case 7: system(OPEN_CMD("emergencies.txt").c_str()); break;
            case 8: system(OPEN_CMD("doctors.txt").c_str()); break;
            case 9: system(OPEN_CMD("patients.txt").c_str()); break;
            case 10: system(OPEN_CMD("appointments.txt").c_str()); break;
            case 11: try_compile_binaries(); break;
            case 0:
                std::cout << "Exiting.\n";
                return 0;
            default:
                std::cout << "Invalid choice.\n";
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return 0;
}

