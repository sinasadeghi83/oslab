#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_CUSTOMERS 5
#define MAX_RESOURCES 5

int available[MAX_RESOURCES];
int maximum[MAX_CUSTOMERS][MAX_RESOURCES];
int allocation[MAX_CUSTOMERS][MAX_RESOURCES];
int need[MAX_CUSTOMERS][MAX_RESOURCES];
int num_resources, num_customers;

bool is_safe_state();
int request_resources(int customer_num, int request[]);
void release_resources(int customer_num, int release[]);

bool is_safe_state() {
    int work[MAX_RESOURCES];
    bool finish[MAX_CUSTOMERS] = {false};

    // copying available array to work
    for (int i = 0; i < num_resources; i++) {
        work[i] = available[i];
    }

    // simulating system execution
    bool progress;
    do {
        progress = false;
        for (int i = 0; i < num_customers; i++) {
            if (!finish[i]) {
                bool can_allocate = true;
                for (int j = 0; j < num_resources; j++) {
                    if (need[i][j] > work[j]) {
                        can_allocate = false;
                        break;
                    }
                }

                if (can_allocate) {
                    for (int j = 0; j < num_resources; j++) {
                        work[j] += allocation[i][j];
                    }
                    finish[i] = true;
                    progress = true;
                }
            }
        }
    } while (progress);

    // check if all proccesses finishes
    for (int i = 0; i < num_customers; i++) {
        if (!finish[i]) {
            return false;
        }
    }
    return true;
}

int request_resources(int customer_num, int request[]) {
    // request prevalidation
    for (int i = 0; i < num_resources; i++) {
        if (request[i] > need[customer_num][i]) {
            printf("This is customer need %d of %d\n", need[customer_num][i], i);
            printf("Error: Process has exceeded its maximum claim.\n");
            return -1;
        }
        if (request[i] > available[i]) {
            printf("Process must wait as resources are not available.\n");
            return -1;
        }
    }

    // allocation
    for (int i = 0; i < num_resources; i++) {
        available[i] -= request[i];
        allocation[customer_num][i] += request[i];
        need[customer_num][i] -= request[i];
    }

    // safe state check
    if (is_safe_state()) {
        printf("Resources allocated successfully.\n");
        return 0;
    } else {
        // rollback
        release_resources(customer_num, request);
        printf("Allocation would lead to unsafe state. Request denied.\n");
        return -1;
    }
}

void release_resources(int customer_num, int release[]) {
    for (int i = 0; i < num_resources; i++) {
        allocation[customer_num][i] -= release[i];
        available[i] += release[i];
        need[customer_num][i] += release[i];
    }
    printf("Resources released successfully.\n");
}

int main() {
    printf("Enter the number of resources: ");
    scanf("%d", &num_resources);
    printf("Enter the number of customers: ");
    scanf("%d", &num_customers);

    printf("Enter the available resources: ");
    for (int i = 0; i < num_resources; i++) {
        scanf("%d", &available[i]);
    }

    printf("Enter the maximum resource matrix: \n");
    for (int i = 0; i < num_customers; i++) {
        for (int j = 0; j < num_resources; j++) {
            scanf("%d", &maximum[i][j]);
            allocation[i][j] = 0;
            need[i][j] = maximum[i][j];
        }
    }

    int choice;
    while (1) {
        printf("\n1. Request resources\n2. Release resources\n3. Exit\nEnter your choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            int customer_num, request[MAX_RESOURCES];
            printf("Enter customer number: ");
            scanf("%d", &customer_num);
            printf("Enter resources to request: ");
            for (int i = 0; i < num_resources; i++) {
                scanf("%d", &request[i]);
            }
            request_resources(customer_num, request);
        } else if (choice == 2) {
            int customer_num, release[MAX_RESOURCES];
            printf("Enter customer number: ");
            scanf("%d", &customer_num);
            printf("Enter resources to release: ");
            for (int i = 0; i < num_resources; i++) {
                scanf("%d", &release[i]);
            }
            release_resources(customer_num, release);
        } else if (choice == 3) {
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }

    return 0;
}
