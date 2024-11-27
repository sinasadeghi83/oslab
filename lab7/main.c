#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

void toggle_case(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (islower(str[i])) {
            str[i] = toupper(str[i]);
        } else if (isupper(str[i])) {
            str[i] = tolower(str[i]);
        }
    }
}

int main() {
    int pipe1[2], pipe2[2];
    int pid;

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe failed");
        return 1;
    }

    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    } else if (pid == 0) {
        close(pipe1[1]);
        close(pipe2[0]);

        char message[100];
        read(pipe1[0], message, sizeof(message));
        toggle_case(message);
        write(pipe2[1], message, strlen(message) + 1);

        close(pipe1[0]);
        close(pipe2[1]);
    } else {
        close(pipe1[0]); 
        close(pipe2[1]);

        char message[] = "This Is First Process";
        char modified_message[100];

        write(pipe1[1], message, strlen(message) + 1);
        read(pipe2[0], modified_message, sizeof(modified_message));

        printf("Original Message: %s\n", message);
        printf("Modified Message: %s\n", modified_message);

        close(pipe1[1]);
        close(pipe2[0]);
    }

    return 0;
}
