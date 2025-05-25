#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to handle each client connection
void *client_thread(void *arg) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("\n Socket creation error \n");
        return NULL;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return NULL;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return NULL;
    }

    // Read server's response
    read(sock, buffer, sizeof(buffer));
    printf("%s\n", buffer);

    // Close the socket
    close(sock);
    return NULL;
}

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <number of clients>\n", argv[0]);
        return -1;
    }

    int num_clients = atoi(argv[1]);
    pthread_t tid[num_clients];

    // Create multiple client connections
    for (int i = 0; i < num_clients; i++) {
        if (pthread_create(&tid[i], NULL, client_thread, NULL) != 0) {
            printf("Failed to create thread\n");
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_clients; i++) {
        pthread_join(tid[i], NULL);
    }

    return 0;
}
