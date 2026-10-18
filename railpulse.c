#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SEATS 100
#define MAX_WAIT  50

typedef struct {
    int pnr;
    char name[50];
    int age;
    char gender;
} Person;

typedef struct Node {
    Person info;
    int seatNo;
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct {
    Person data[MAX_WAIT];
    int front;
    int count;
} WaitQueue;

typedef struct Train {
    int trainNo;
    char name[50];
    char source[30];
    char destination[30];
    int totalSeats;
    int availableSeats;
    char seatTaken[MAX_SEATS + 1];
    Node *head;
    Node *tail;
    WaitQueue wq;
    int routeCount;
    char routeStations[20][30];
    int currentStationIndex;
    char runningStatus[20];
    struct Train *next;
} Train;

Train *trainHead = NULL;
Train *trainTail = NULL;
int nextPNR = 5001;

void readString(const char *prompt, char *dest, int size) {
    printf("%s", prompt);
    if (fgets(dest, size, stdin) == NULL)
        dest[0] = '\0';
    dest[strcspn(dest, "\n")] = '\0';
}

int readInt(const char *prompt) {
    char buf[50];
    int v;
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof(buf), stdin) == NULL)
            return 0;
        if (sscanf(buf, "%d", &v) == 1)
            return v;
        printf("Invalid input! Enter a number.\n");
    }
}

void initQueue(WaitQueue *q) {
    q->front = 0;
    q->count = 0;
}

int isQueueEmpty(WaitQueue *q) {
    return q->count == 0;
}

int isQueueFull(WaitQueue *q) {
    return q->count == MAX_WAIT;
}

int enqueue(WaitQueue *q, Person p) {
    if (isQueueFull(q))
        return 0;
    int rear = (q->front + q->count) % MAX_WAIT;
    q->data[rear] = p;
    q->count++;
    return 1;
}

int dequeue(WaitQueue *q, Person *out) {
    if (isQueueEmpty(q))
        return 0;
    *out = q->data[q->front];
    q->front = (q->front + 1) % MAX_WAIT;
    q->count--;
    return 1;
}

int removeFromQueue(WaitQueue *q, int pnr, Person *out) {
    for (int i = 0; i < q->count; i++) {
        int idx = (q->front + i) % MAX_WAIT;
        if (q->data[idx].pnr == pnr) {
            *out = q->data[idx];
            for (int j = i; j < q->count - 1; j++) {
                int cur = (q->front + j) % MAX_WAIT;
                int nxt = (q->front + j + 1) % MAX_WAIT;
                q->data[cur] = q->data[nxt];
            }
            q->count--;
            return 1;
        }
    }
    return 0;
}

void appendNode(Train *t, Node *n) {
    n->next = NULL;
    n->prev = t->tail;
    if (t->tail == NULL)
        t->head = n;
    else
        t->tail->next = n;
    t->tail = n;
}

void unlinkNode(Train *t, Node *n) {
    if (n->prev != NULL)
        n->prev->next = n->next;
    else
        t->head = n->next;
    if (n->next != NULL)
        n->next->prev = n->prev;
    else
        t->tail = n->prev;
    n->prev = n->next = NULL;
}

Node *findNode(Train *t, int pnr) {
    Node *cur = t->head;
    while (cur != NULL) {
        if (cur->info.pnr == pnr)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

void confirmPassenger(Train *t, Person p, int seat) {
    Node *n = (Node *)malloc(sizeof(Node));
    if (n == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }
    n->info = p;
    n->seatNo = seat;
    t->seatTaken[seat] = 1;
    t->availableSeats--;
    appendNode(t, n);
}

int getFreeSeat(Train *t) {
    for (int s = 1; s <= t->totalSeats; s++)
        if (!t->seatTaken[s])
            return s;
    return 0;
}

Train *findTrain(int no) {
    Train *t = trainHead;
    while (t != NULL) {
        if (t->trainNo == no)
            return t;
        t = t->next;
    }
    return NULL;
}

void addTrain() {
    int no = readInt("Enter train number      : ");
    if (findTrain(no) != NULL) {
        printf("Train already exists!\n");
        return;
    }
    Train *t = (Train *)calloc(1, sizeof(Train));
    if (t == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }
    t->trainNo = no;
    readString("Enter train name        : ", t->name, sizeof(t->name));
    readString("Enter source station    : ", t->source, sizeof(t->source));
    readString("Enter destination       : ", t->destination, sizeof(t->destination));
    int intermediateCount = readInt("Number of intermediate stations (0-18): ");
    if (intermediateCount < 0 || intermediateCount > 18) {
        printf("Intermediate stations must be between 0 and 18!\n");
        free(t);
        return;
    }
    t->routeCount = intermediateCount + 2;
    strncpy(t->routeStations[0], t->source, sizeof(t->routeStations[0]) - 1);
    t->routeStations[0][sizeof(t->routeStations[0]) - 1] = '\0';
    for (int i = 0; i < intermediateCount; i++) {
        char prompt[80];
        snprintf(prompt, sizeof(prompt), "Intermediate station %d: ", i + 1);
        readString(prompt, t->routeStations[i + 1], sizeof(t->routeStations[i + 1]));
    }
    strncpy(t->routeStations[t->routeCount - 1], t->destination, sizeof(t->routeStations[0]) - 1);
    t->routeStations[t->routeCount - 1][sizeof(t->routeStations[0]) - 1] = '\0';
    t->currentStationIndex = 0;
    strcpy(t->runningStatus, "On time");
    t->totalSeats = readInt("Enter total seats (max 100): ");
    if (t->totalSeats <= 0 || t->totalSeats > MAX_SEATS) {
        printf("Seats must be between 1 and %d!\n", MAX_SEATS);
        free(t);
        return;
    }
    t->availableSeats = t->totalSeats;
    initQueue(&t->wq);
    if (trainTail == NULL)
        trainHead = t;
    else
        trainTail->next = t;
    trainTail = t;
    printf("Train added successfully!\n");
}

void displayTrains() {
    if (trainHead == NULL) {
        printf("No trains available.\n");
        return;
    }
    printf("\n%-8s %-20s %-15s %-15s %-6s %-6s %-7s\n",
           "TrainNo", "Name", "From", "To", "Total", "Free", "Waiting");
    printf("----------------------------------------------------------------------------\n");
    for (Train *t = trainHead; t != NULL; t = t->next)
        printf("%-8d %-20s %-15s %-15s %-6d %-6d %-7d\n",
               t->trainNo, t->name, t->source, t->destination,
               t->totalSeats, t->availableSeats, t->wq.count);
}

void bookTicket() {
    Train *t = findTrain(readInt("Enter train number to book : "));
    if (t == NULL) {
        printf("Train not found!\n");
        return;
    }
    Person p;
    readString("Passenger name   : ", p.name, sizeof(p.name));
    p.age = readInt("Passenger age    : ");
    char g[10];
    readString("Gender (M/F/O)   : ", g, sizeof(g));
    p.gender = g[0];
    if (t->availableSeats == 0 && isQueueFull(&t->wq)) {
        printf("Train and waiting list are FULL. Booking rejected.\n");
        return;
    }
    p.pnr = nextPNR++;

    if (t->availableSeats > 0) {
        int seat = getFreeSeat(t);
        confirmPassenger(t, p, seat);
        printf("\nTicket CONFIRMED!  PNR: %d  Seat No: %d\n", p.pnr, seat);
    } else {
        enqueue(&t->wq, p);
        printf("\nTrain full. WAITING LIST.  PNR: %d  Position: %d\n", p.pnr, t->wq.count);
    }
}

void cancelTicket() {
    Train *t = findTrain(readInt("Enter train number : "));
    if (t == NULL) {
        printf("Train not found!\n");
        return;
    }
    int pnr = readInt("Enter PNR to cancel : ");

    Node *n = findNode(t, pnr);
    if (n != NULL) {
        int seat = n->seatNo;
        printf("Ticket %d (%s) cancelled.\n", n->info.pnr, n->info.name);
        unlinkNode(t, n);
        free(n);
        t->seatTaken[seat] = 0;
        t->availableSeats++;

        Person next;
        if (dequeue(&t->wq, &next)) {
            confirmPassenger(t, next, seat);
            printf("Waiting passenger %s (PNR %d) is now CONFIRMED on seat %d.\n",
                   next.name, next.pnr, seat);
        }
        return;
    }

    Person removed;
    if (removeFromQueue(&t->wq, pnr, &removed)) {
        printf("Waiting-list ticket %d (%s) cancelled.\n", removed.pnr, removed.name);
        return;
    }
    printf("PNR not found on this train!\n");
}

void displayConfirmed() {
    Train *t = findTrain(readInt("Enter train number : "));
    if (t == NULL) {
        printf("Train not found!\n");
        return;
    }
    if (t->head == NULL) {
        printf("No confirmed passengers.\n");
        return;
    }
    printf("\nConfirmed passengers of train %d (%s)\n", t->trainNo, t->name);
    printf("%-6s %-20s %-5s %-7s %-5s\n", "PNR", "Name", "Age", "Gender", "Seat");
    printf("----------------------------------------------------\n");
    for (Node *n = t->head; n != NULL; n = n->next)
        printf("%-6d %-20s %-5d %-7c %-5d\n",
               n->info.pnr, n->info.name, n->info.age, n->info.gender, n->seatNo);
}

void displayWaiting() {
    Train *t = findTrain(readInt("Enter train number : "));
    if (t == NULL) {
        printf("Train not found!\n");
        return;
    }
    if (isQueueEmpty(&t->wq)) {
        printf("Waiting list is empty.\n");
        return;
    }
    printf("\nWaiting list of train %d (%s)\n", t->trainNo, t->name);
    printf("%-4s %-6s %-20s %-5s %-7s\n", "No.", "PNR", "Name", "Age", "Gender");
    printf("------------------------------------------------\n");
    for (int i = 0; i < t->wq.count; i++) {
        Person *p = &t->wq.data[(t->wq.front + i) % MAX_WAIT];
        printf("%-4d %-6d %-20s %-5d %-7c\n", i + 1, p->pnr, p->name, p->age, p->gender);
    }
}

void displayReverse() {
    Train *t = findTrain(readInt("Enter train number : "));
    if (t == NULL) {
        printf("Train not found!\n");
        return;
    }
    if (t->tail == NULL) {
        printf("No confirmed passengers.\n");
        return;
    }
    printf("\nPassengers in reverse booking order:\n");
    for (Node *n = t->tail; n != NULL; n = n->prev)
        printf("PNR %d  %-20s Seat %d\n", n->info.pnr, n->info.name, n->seatNo);
}

void searchByPNR() {
    int pnr = readInt("Enter PNR to search : ");
    for (Train *t = trainHead; t != NULL; t = t->next) {
        Node *n = findNode(t, pnr);
        if (n != NULL) {
            printf("\nPNR %d: %s, Age %d, Train %d (%s), Seat %d -> CONFIRMED\n",
                   pnr, n->info.name, n->info.age, t->trainNo, t->name, n->seatNo);
            return;
        }
        for (int i = 0; i < t->wq.count; i++) {
            Person *p = &t->wq.data[(t->wq.front + i) % MAX_WAIT];
            if (p->pnr == pnr) {
                printf("\nPNR %d: %s, Age %d, Train %d (%s) -> WAITING (position %d)\n",
                       pnr, p->name, p->age, t->trainNo, t->name, i + 1);
                return;
            }
        }
    }
    printf("PNR not found!\n");
}


void checkTrainLocation(void) {
    int no = readInt("Enter train number to track: ");
    Train *t = findTrain(no);
    if (t == NULL) { printf("Train not found!\n"); return; }
    printf("\n===== TRAIN LOCATION (DEMO DATA) =====\n");
    printf("Train      : %d - %s\n", t->trainNo, t->name);
    printf("Status     : %s\n", t->runningStatus);
    printf("Last report: %s\n", t->routeStations[t->currentStationIndex]);
    if (t->currentStationIndex + 1 < t->routeCount)
        printf("Next stop  : %s\n", t->routeStations[t->currentStationIndex + 1]);
    else printf("Next stop  : Destination reached\n");
    printf("Route      : ");
    for (int i = 0; i < t->routeCount; i++) {
        if (i) printf(" -> ");
        if (i == t->currentStationIndex) printf("[%s]", t->routeStations[i]);
        else printf("%s", t->routeStations[i]);
    }
    printf("\nNote: manual demo data, not GPS/live railway data.\n");
}

void updateTrainLocation(void) {
    int no = readInt("Enter train number to update: ");
    Train *t = findTrain(no);
    if (t == NULL) { printf("Train not found!\n"); return; }
    printf("\nRoute for %d - %s\n", t->trainNo, t->name);
    for (int i = 0; i < t->routeCount; i++)
        printf("%2d. %s%s\n", i + 1, t->routeStations[i],
               i == t->currentStationIndex ? "  <current>" : "");
    int choice = readInt("Set current station number: ");
    if (choice < 1 || choice > t->routeCount) { printf("Invalid station number!\n"); return; }
    t->currentStationIndex = choice - 1;
    readString("Running status (On time/Delayed/Stopped): ", t->runningStatus, sizeof(t->runningStatus));
    if (t->runningStatus[0] == '\0') strcpy(t->runningStatus, "On time");
    printf("Demo location updated to %s.\n", t->routeStations[t->currentStationIndex]);
}

void freeAll() {
    while (trainHead != NULL) {
        Train *t = trainHead;
        trainHead = trainHead->next;
        Node *n = t->head;
        while (n != NULL) {
            Node *tmp = n;
            n = n->next;
            free(tmp);
        }
        free(t);
    }
    trainTail = NULL;
}

int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
    int choice;

    do {
        printf("\n========= TRAIN RESERVATION SYSTEM =========\n");
        printf("1. Add Train\n");
        printf("2. Display All Trains\n");
        printf("3. Book Ticket\n");
        printf("4. Cancel Ticket\n");
        printf("5. Display Confirmed Passengers\n");
        printf("6. Display Waiting List\n");
        printf("7. Search Ticket by PNR\n");
        printf("8. Display Passengers in Reverse Order\n");
        printf("9. Check Train Location (demo)\n");
        printf("10. Update Train Location (manual demo update)\n");
        printf("0. Exit\n");
        choice = readInt("Enter your choice: ");

        switch (choice) {
            case 1: addTrain();         break;
            case 2: displayTrains();    break;
            case 3: bookTicket();       break;
            case 4: cancelTicket();     break;
            case 5: displayConfirmed(); break;
            case 6: displayWaiting();   break;
            case 7: searchByPNR();      break;
            case 8: displayReverse();   break;
            case 9: checkTrainLocation(); break;
            case 10: updateTrainLocation(); break;
            case 0: printf("Thank you for using the system!\n"); break;
            default: printf("Invalid choice! Try again.\n");
        }
    } while (choice != 0);

    freeAll();
    return 0;
}