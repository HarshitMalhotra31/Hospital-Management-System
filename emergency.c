#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_EMERG 100
#define STR_LEN 256
#define FILE_NAME "emergencies.txt"

typedef struct {
    char eid[64];
    int patientID;
    char patientName[STR_LEN];
    char severity[32];
    int priority; // 1=Critical,2=High,3=Moderate,4=Low
    char doctor[STR_LEN];
    char timestamp[32];
    char notes[512];
    int treated;
} Emergency;

Emergency heap[MAX_EMERG];
int heapSize = 0;

// --- Utility functions ---
void remove_newline(char *s){ s[strcspn(s,"\n")] = '\0'; }

int get_priority(const char *sev) {
    if (strcasecmp(sev, "Critical")==0) return 1;
    if (strcasecmp(sev, "High")==0) return 2;
    if (strcasecmp(sev, "Moderate")==0) return 3;
    if (strcasecmp(sev, "Low")==0) return 4;
    return 5;
}

void gen_timestamp(char *out, size_t n) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(out, n, "%02d/%02d/%04d %02d:%02d:%02d",
             tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

// --- Priority Queue (Min-Heap by priority) ---
void swap(Emergency *a, Emergency *b) {
    Emergency temp = *a; *a = *b; *b = temp;
}

void heapify_up(int i) {
    if (i == 0) return;
    int parent = (i - 1) / 2;
    if (heap[i].priority < heap[parent].priority) {
        swap(&heap[i], &heap[parent]);
        heapify_up(parent);
    }
}

void heapify_down(int i) {
    int left = 2*i + 1, right = 2*i + 2, smallest = i;
    if (left < heapSize && heap[left].priority < heap[smallest].priority)
        smallest = left;
    if (right < heapSize && heap[right].priority < heap[smallest].priority)
        smallest = right;
    if (smallest != i) {
        swap(&heap[i], &heap[smallest]);
        heapify_down(smallest);
    }
}

void push(Emergency e) {
    if (heapSize >= MAX_EMERG) {
        printf("Queue full.\n");
        return;
    }
    heap[heapSize] = e;
    heapify_up(heapSize);
    heapSize++;
}

Emergency pop() {
    Emergency top = heap[0];
    heap[0] = heap[--heapSize];
    heapify_down(0);
    return top;
}

Emergency peek() {
    return heap[0];
}

// --- File persistence ---
void save_to_file() {
    FILE *f = fopen(FILE_NAME, "w");
    if (!f) return;
    for (int i = 0; i < heapSize; i++) {
        Emergency *e = &heap[i];
        fprintf(f, "%s;%d;%s;%s;%s;%s;%s;%d\n",
            e->eid, e->patientID, e->patientName, e->severity,
            e->doctor, e->timestamp, e->notes, e->treated);
    }
    fclose(f);
}

void load_from_file() {
    heapSize = 0;
    FILE *f = fopen(FILE_NAME, "r");
    if (!f) return;
    Emergency e;
    while (fscanf(f, "%63[^;];%d;%255[^;];%31[^;];%255[^;];%31[^;];%511[^;];%d\n",
                  e.eid, &e.patientID, e.patientName, e.severity,
                  e.doctor, e.timestamp, e.notes, &e.treated) == 8) {
        e.priority = get_priority(e.severity);
        heap[heapSize++] = e;
    }
    fclose(f);
    // re-heapify after loading
    for (int i = heapSize/2 - 1; i >= 0; i--)
        heapify_down(i);
}

// --- Core operations ---
void add_emergency() {
    Emergency e;
    snprintf(e.eid, sizeof(e.eid), "%ld", time(NULL));

    printf("Enter Patient ID: ");
    scanf("%d", &e.patientID);
    getchar();
    printf("Enter Patient Name: ");
    fgets(e.patientName, sizeof(e.patientName), stdin); remove_newline(e.patientName);

    printf("Severity (Critical/High/Moderate/Low): ");
    fgets(e.severity, sizeof(e.severity), stdin); remove_newline(e.severity);
    e.priority = get_priority(e.severity);

    printf("Assign Doctor: ");
    fgets(e.doctor, sizeof(e.doctor), stdin); remove_newline(e.doctor);

    printf("Notes: ");
    fgets(e.notes, sizeof(e.notes), stdin); remove_newline(e.notes);

    gen_timestamp(e.timestamp, sizeof(e.timestamp));
    e.treated = 0;

    push(e);
    save_to_file();
    printf("\nEmergency added with ID %s and priority %d\n", e.eid, e.priority);
}

void list_emergencies() {
    if (heapSize == 0) { printf("No emergencies.\n"); return; }
    printf("\n%-12s %-8s %-20s %-10s %-12s %-20s %-8s\n",
           "EID", "PatID", "Patient", "Severity", "Doctor", "Time", "Treated");
    printf("----------------------------------------------------------------------------\n");
    for (int i = 0; i < heapSize; i++) {
        Emergency *e = &heap[i];
        printf("%-12s %-8d %-20s %-10s %-12s %-20s %-8s\n",
               e->eid, e->patientID, e->patientName, e->severity,
               e->doctor, e->timestamp, e->treated ? "Yes":"No");
    }
}

void treat_next_emergency() {
    if (heapSize == 0) { printf("No emergencies in queue.\n"); return; }
    Emergency e = pop();
    e.treated = 1;
    printf("\n--- Treating Emergency ---\n");
    printf("EID: %s\nPatient: %s\nSeverity: %s\nDoctor: %s\nNotes: %s\n",
           e.eid, e.patientName, e.severity, e.doctor, e.notes);
    printf("Status: Treated\n");
    save_to_file();
}

void delete_emergency() {
    char id[64];
    printf("Enter EID to delete: ");
    fgets(id, sizeof(id), stdin); remove_newline(id);
    int found = 0;
    for (int i = 0; i < heapSize; i++) {
        if (strcmp(heap[i].eid, id) == 0) {
            found = 1;
            heap[i] = heap[--heapSize];
            heapify_down(i);
            save_to_file();
            printf("Deleted emergency %s\n", id);
            break;
        }
    }
    if (!found) printf("Not found.\n");
}

// --- Menu ---
void menu() {
    load_from_file();
    int ch;
    while (1) {
        printf("\n=== Emergency Management (Priority Queue) ===\n");
        printf("1. Register Emergency\n");
        printf("2. List Emergencies\n");
        printf("3. Treat Next (Highest Priority)\n");
        printf("4. Delete Emergency by ID\n");
        printf("0. Exit\nChoice: ");
        if (scanf("%d", &ch) != 1) { while(getchar()!='\n'); continue; }
        getchar();
        switch(ch) {
            case 1: add_emergency(); break;
            case 2: list_emergencies(); break;
            case 3: treat_next_emergency(); break;
            case 4: delete_emergency(); break;
            case 0: return;
            default: printf("Invalid choice.\n");
        }
    }
}

int main() {
    menu();
    return 0;
}
