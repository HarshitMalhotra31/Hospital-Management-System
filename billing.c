#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define STR_LEN 256
#define BILLS_FILE "bills.txt"

typedef struct {
    long billID;
    int patientID;
    char patientName[STR_LEN];
    char date[16]; // dd/mm/yyyy
    double subtotal;
    double tax;
    double total;
    char items[1024]; // items separated by |
} Bill;

void remove_newline(char *s) { s[strcspn(s, "\n")] = '\0'; }

long gen_bill_id() {
    return (long) time(NULL);
}

void ensure_file_exists(const char *fname) {
    FILE *f = fopen(fname, "a");
    if (f) fclose(f);
}

void add_bill() {
    ensure_file_exists(BILLS_FILE);
    Bill b;
    char buf[STR_LEN];
    b.billID = gen_bill_id();

    printf("Enter Patient ID: ");
    if (scanf("%d", &b.patientID) != 1) { while (getchar() != '\n'); printf("Invalid\n"); return; }
    getchar();

    printf("Enter Patient Name: ");
    fgets(b.patientName, STR_LEN, stdin); 
    remove_newline(b.patientName);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(b.date, sizeof(b.date), "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

    b.subtotal = 0.0;
    b.items[0] = '\0';

    while (1) {
        char desc[256];
        int qty;
        double unit;
        printf("\nAdd item? (y/n): ");
        if (fgets(buf, sizeof(buf), stdin) == NULL) break;
        remove_newline(buf);
        if (buf[0] != 'y' && buf[0] != 'Y') break;

        printf("Description: ");
        fgets(desc, sizeof(desc), stdin);
        remove_newline(desc);
        printf("Quantity: ");
        if (scanf("%d", &qty) != 1) { while (getchar() != '\n'); printf("Invalid qty\n"); continue; }
        getchar();
        printf("Unit price: ");
        if (scanf("%lf", &unit) != 1) { while (getchar() != '\n'); printf("Invalid price\n"); continue; }
        getchar();

        double line = qty * unit;
        b.subtotal += line;

        char itemline[512];
        snprintf(itemline, sizeof(itemline), "%s x%d @%.2f = %.2f", desc, qty, unit, line);
        if (b.items[0] != '\0') strncat(b.items, "|", sizeof(b.items) - strlen(b.items) - 1);
        strncat(b.items, itemline, sizeof(b.items) - strlen(b.items) - 1);

        printf("Item added.\n");
    }

    b.tax = b.subtotal * 0.05; // 5% tax
    b.total = b.subtotal + b.tax;

    FILE *f = fopen(BILLS_FILE, "a");
    if (!f) { perror("Unable to open bills file"); return; }

    fprintf(f, "%ld;%d;%s;%s;%.2f;%.2f;%.2f;%s\n",
            b.billID, b.patientID, b.patientName, b.date, b.subtotal, b.tax, b.total, b.items);
    fclose(f);
    printf("\nBill saved with ID: %ld\n", b.billID);
}

void list_bills() {
    ensure_file_exists(BILLS_FILE);
    FILE *f = fopen(BILLS_FILE, "r");
    if (!f) { perror("open"); return; }
    char line[2048];
    printf("\n%-12s %-10s %-20s %-12s %-10s\n", "BillID", "PatID", "PatientName", "Date", "Total");
    printf("-----------------------------------------------------------------\n");
    while (fgets(line, sizeof(line), f)) {
        char *p = line;
        char *token;
        token = strtok(p, ";"); if (!token) continue; printf("%-12s ", token); // billID
        token = strtok(NULL, ";"); if (!token) continue; printf("%-10s ", token); // patientID
        token = strtok(NULL, ";"); if (!token) continue; {
            char name[128]; strncpy(name, token, 127); name[127] = 0; printf("%-20s ", name);
        }
        token = strtok(NULL, ";"); if (!token) continue; printf("%-12s ", token); // date
        token = strtok(NULL, ";"); if (!token) continue; // subtotal
        token = strtok(NULL, ";"); if (!token) continue; // tax
        token = strtok(NULL, ";"); if (!token) continue; printf("%-10s\n", token); // total
    }
    fclose(f);
}

void view_bill() {
    ensure_file_exists(BILLS_FILE);
    char buf[64];
    printf("Enter Bill ID to view: ");
    if (fgets(buf, sizeof(buf), stdin) == NULL) return;
    remove_newline(buf);
    long bid = atol(buf);
    FILE *f = fopen(BILLS_FILE, "r");
    if (!f) { perror("open"); return; }
    char line[2048];
    while (fgets(line, sizeof(line), f)) {
        char copy[2048]; strcpy(copy, line);
        char *token = strtok(copy, ";");
        if (!token) continue;
        if (atol(token) != bid) continue;

        char *billID = token;
        char *patID = strtok(NULL, ";");
        char *pname = strtok(NULL, ";");
        char *date = strtok(NULL, ";");
        char *subtotal = strtok(NULL, ";");
        char *tax = strtok(NULL, ";");
        char *total = strtok(NULL, ";");
        char *items = strtok(NULL, ";\n");

        printf("\nBill ID   : %s\nPatient ID: %s\nPatient   : %s\nDate      : %s\n",
               billID, patID ? patID : "", pname ? pname : "", date ? date : "");
        printf("Subtotal  : %s\nTax (5%%)  : %s\nTotal     : %s\n",
               subtotal ? subtotal : "0.00", tax ? tax : "0.00", total ? total : "0.00");
        printf("\nItems:\n");
        if (items) {
            char *it = strtok(items, "|");
            while (it) {
                printf(" - %s\n", it);
                it = strtok(NULL, "|");
            }
        }
        fclose(f);
        return;
    }
    fclose(f);
    printf("Bill not found.\n");
}

void delete_bill() {
    ensure_file_exists(BILLS_FILE);
    char buf[64];
    printf("Enter Bill ID to delete: ");
    if (fgets(buf, sizeof(buf), stdin) == NULL) return;
    remove_newline(buf);
    long bid = atol(buf);
    FILE *f = fopen(BILLS_FILE, "r");
    if (!f) { perror("open"); return; }
    FILE *tmp = fopen("bills.tmp", "w");
    if (!tmp) { fclose(f); perror("tmp"); return; }
    char line[2048];
    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        char copy[2048]; strcpy(copy, line);
        char *token = strtok(copy, ";");
        if (!token) continue;
        if (atol(token) == bid) { found = 1; continue; }
        fputs(line, tmp);
    }
    fclose(f);
    fclose(tmp);
    if (found) {
        remove(BILLS_FILE);
        rename("bills.tmp", BILLS_FILE);
        printf("Bill %ld deleted.\n", bid);
    } else {
        remove("bills.tmp");
        printf("Bill not found.\n");
    }
}

void bills_menu() {
    while (1) {
        printf("\n=== Billing Module ===\n");
        printf("1. Add Bill\n2. List Bills\n3. View Bill\n4. Delete Bill\n0. Exit\nChoice: ");
        int ch;
        if (scanf("%d", &ch) != 1) { while (getchar() != '\n'); printf("Invalid\n"); continue; }
        getchar();
        switch (ch) {
            case 1: add_bill(); break;
            case 2: list_bills(); break;
            case 3: view_bill(); break;
            case 4: delete_bill(); break;
            case 0: return;
            default: printf("Invalid\n");
        }
    }
}

int main() {
    ensure_file_exists(BILLS_FILE);
    bills_menu();
    printf("Exiting Billing module.\n");
    return 0;
}

