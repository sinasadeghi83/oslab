#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define PORT 8088
#define MAX_CLIENTS 20
#define CLIENT_ID 0
#define PRDS_ID 1
#define UIDX_ID 2
#define MAX_PRODUCTS 20

typedef struct
{
    char name[16];
    int count;
} Product;

typedef struct
{
    int id;
    int prdLen;
    char name[16];
} Client;

int *uIDX;
int clid, prdid, uidxid;

Client *clients;
Product **prds;

void initialShm();
void handle_client_command(int client_socket, char *command, char *clientName);
int find_uidx(char clName[16]);
int find_product(char name[16], char clName[16], int *idx);
int rem_product(char name[16], char clName[16]);
int add_product(char name[16], int amount, char clName[16]);
int create_product(char name[16], int count, char clName[16]);

int find_uidx(char clName[16])
{
    printf("\nuidx is %d\n", *uIDX);
    for (int i = 0; i < *uIDX; i++)
    {
        if (strcmp(clName, clients[i].name) == 0)
        {
            return clients[i].id;
        }
    }

    return -1;
}

int find_product(char name[16], char clName[16], int *idx)
{
    int uidx = find_uidx(clName);
    if (uidx == -1)
    {
        printf("USER NOT FOUND");
        return -1;
    }
    int len = clients[uidx].prdLen;
    if (len == 0)
    {
        printf("USER LEN IS ZERO");
        return -1;
    }

    for (int i = 0; i < len; i++)
    {
        Product prd = prds[uidx][i];
        if (strcmp(name, prd.name) == 0)
        {
            *idx = i;
            return 0;
        }
    }

    return -1;
}

int rem_product(char name[16], char clName[16])
{
    int uidx = find_uidx(clName);

    if (uidx == -1)
    {
        return -1;
    }

    int pidx;
    if (find_product(name, clName, &pidx) == -1)
        return -1;
    
    for(int i = pidx+1; i < clients[uidx].prdLen; i++){
        prds[uidx][i-1] = prds[uidx][i];
    }

    clients[uidx].prdLen-=1;

    return 0;
}

int send_product(char name[16], int amount, char clName[16], char tgcl[16])
{
    int uidx = find_uidx(clName);
    if (uidx == -1)
    {
        return -1;
    }

    int tgidx = find_uidx(tgcl);
    if (tgidx == -1)
    {
        return -1;
    }

    int pidx;
    if (find_product(name, clName, &pidx) < 0)
    {
        return -1;
    }

    if (amount <= 0)
    {
        return -1;
    }
    
    Product *prd = &prds[uidx][pidx];
    if (prd->count - amount < 0)
    {
        return -1;
    }

    prd->count -= amount;

    int res;
    if ((res = add_product(name, amount, tgcl)) < 0)
    {
        return create_product(name, amount, tgcl);
    }

    return res;
}

int add_product(char name[16], int amount, char clName[16])
{
    int uidx = find_uidx(clName);

    if (uidx == -1)
    {
        return -1;
    }

    int pidx;
    if (find_product(name, clName, &pidx) < 0)
    {
        printf("\nPRODUCT NOT FOUND\n");
        return -1;
    }

    Product *prd = &prds[uidx][pidx];
    if (prd->count + amount < 0)
    {
        printf("\nPRODUCT AMOUNT INVALID: %d\n", amount);
        return -1;
    }

    prd->count += amount;

    return 0;
}

int list_products(char clName[16], char *result)
{
    int uidx = find_uidx(clName);

    if (uidx == -1)
    {
        return -1;
    }

    strcpy(result, "Product list:\n");

    int prdlen = clients[uidx].prdLen;
    for(int i =0; i < prdlen; i++)
    {
        Product prd = prds[uidx][i];
        char line[64];
        snprintf(line, sizeof(line), "Product: %s, Count: %d\n", prd.name, prd.count);
        strcat(result, line);
    }

    return 0;
}

int create_product(char name[16], int count, char clName[16])
{
    int uidx = find_uidx(clName);

    if (uidx == -1)
    {
        return -1;
    }

    if (count < 0)
    {
        return -1;
    }

    int pidx;
    if (find_product(name, clName, &pidx) == 0)
    {
        return -1;
    }

    Product prd;
    strcpy(prd.name, name);
    prd.count = count;

    prds[uidx][clients[uidx].prdLen] = prd;
    clients[uidx].prdLen += 1;
    return 0;
}

int create_user(char clName[16])
{
    int uidx = find_uidx(clName);

    if (uidx != -1)
    {
        return -1;
    }

    Client cl;
    uidx = *uIDX;
    cl.id = uidx;
    strcpy(cl.name, clName);
    clients[uidx] = cl;
    *uIDX += 1;
    return 0;
}

int main(int argc, char const *argv[])
{
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    // creates socket file descriptor
    int server_fd;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT); // host to network -- coverts the ending of the given integer
    const int addrlen = sizeof(address);
    // binding
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    // listening on server socket with backlog size 3.
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    initialShm();

    printf("Listen on %s:%d\n", inet_ntoa(address.sin_addr),
           ntohs(address.sin_port));
    while (1)
    {
        // accepting client
        // accept returns client socket and fills given address and addrlen with client address information.
        int client_socket, valread;
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address,
                                    (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        int pid = fork();

        if (pid > 0)
        {
            continue;
        }
        printf("Hello client %s:%d\n", inet_ntoa(address.sin_addr),
               ntohs(address.sin_port));
        char clientName[16];
        valread = read(client_socket, clientName, 16);
        create_user(clientName);
        // reads a buffer with maximum size 1024 from socket.
        while ((valread = read(client_socket, buffer, 1024)) > 0)
        {
            buffer[valread] = '\0';
            printf("\n this is your command: %s\n", buffer);
            handle_client_command(client_socket, buffer, clientName);
        }
        if (valread < 0)
        {
            perror("read");
            close(client_socket);
        }
        exit(0);
    }
    return 0;
}

void handle_client_command(int client_socket, char *command, char *clientName)
{
    char response[1024];
    char cmd[16], arg1[16];
    int amount = 0;
    int result = -1;

    sscanf(command, "%s %s %d", cmd, arg1, &amount);

    if (strcmp(cmd, "list") == 0)
    {
        result = list_products(clientName, response);
        if (result != 0)
        {
            snprintf(response, sizeof(response), "Failed to retrieve product list\n");
        }
    }
    else if (strcmp(cmd, "create") == 0)
    {
        if (create_product(arg1, amount, clientName) == 0)
        {
            snprintf(response, sizeof(response), "Product %s created with count %d\n", arg1, amount);
        }
        else
        {
            snprintf(response, sizeof(response), "Failed to create product %s with count %d for %s\n", arg1, amount, clientName);
        }
    }
    else if (strcmp(cmd, "add") == 0)
    {
        // به موجودی اضافه کند
        if (add_product(arg1, amount, clientName) == 0)
        {
            snprintf(response, sizeof(response), "Added %d to product %s\n", amount, arg1);
        }
        else
        {
            snprintf(response, sizeof(response), "Failed to add to product %s\n", arg1);
        }
    }
    else if (strcmp(cmd, "reduce") == 0)
    {
        // از موجودی کم کند
        if (add_product(arg1, -amount, clientName) == 0)
        {
            snprintf(response, sizeof(response), "Reduced %d from product %s\n", amount, arg1);
        }
        else
        {
            snprintf(response, sizeof(response), "Failed to reduce product %s\n", arg1);
        }
    }
    else if (strcmp(cmd, "remove") == 0)
    {
        // محصول را حذف کند
        if (rem_product(arg1, clientName) == 0)
        {
            snprintf(response, sizeof(response), "Product %s removed\n", arg1);
        }
        else
        {
            snprintf(response, sizeof(response), "Failed to remove product %s\n", arg1);
        }
    }
    else if (strcmp(cmd, "quit") == 0)
    {
        // کلاینت را از برنامه خارج کند
        strcpy(response, "Goodbye!\n");
        send(client_socket, response, strlen(response), 0);
        close(client_socket);
        exit(0);
    }
    else
    {
        // دستور نامعتبر
        strcpy(response, "Invalid command\n");
    }

    send(client_socket, response, strlen(response), 0);
}

void initialShm()
{
    key_t kClients;

    kClients = ftok("test.txt", CLIENT_ID);
    if (kClients == -1)
    {
        perror("ftok");
        exit(1);
    }

    clid = shmget(kClients, MAX_CLIENTS * sizeof(Client), 0644 | IPC_CREAT);
    if (clid == -1)
    {
        perror("shmget");
        exit(1);
    }

    clients = (Client *)shmat(clid, 0, 0);
    if (clients == (void *)-1)
    {
        perror("shmat");
        exit(1);
    }

    key_t kPrds;

    kPrds = ftok("test.txt", PRDS_ID);
    if (kPrds == -1)
    {
        perror("ftok");
        exit(1);
    }
    prdid = shmget(kPrds, MAX_CLIENTS * MAX_PRODUCTS * sizeof(Product), 0644 | IPC_CREAT);
    if (prdid == -1)
    {
        perror("shmget");
        exit(1);
    }

    Product *products_shared = (Product *)shmat(prdid, 0, 0);
    if (products_shared == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    // Allocate memory for the array of pointers to each client's product array
    prds = (Product **)malloc(MAX_CLIENTS * sizeof(Product *));
    if (prds == NULL) {
        perror("malloc");
        exit(1);
    }

    // Set up prds as a 2D array of pointers into the shared memory block
    for (int i = 0; i < MAX_CLIENTS; i++) {
        prds[i] = &products_shared[i * MAX_PRODUCTS];
    }

    key_t kuidx;

    kuidx = ftok("test.txt", UIDX_ID);
    if (kuidx == -1)
    {
        perror("ftok");
        exit(1);
    }
    uidxid = shmget(kuidx, sizeof(int), 0644 | IPC_CREAT);
    if (prdid == -1)
    {
        perror("shmget");
        exit(1);
    }

    uIDX = (int *)shmat(uidxid, 0, 0);
    if (uIDX == (void *)-1)
    {
        perror("shmat");
        exit(1);
    }
    *uIDX = 0;
}