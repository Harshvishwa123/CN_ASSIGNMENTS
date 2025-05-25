#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number of clients>\n", argv[0]);
        return 1;
    }

    int num_clients = atoi(argv[1]);
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    for (int i = 0; i < num_clients; i++) {
        // Create socket
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("\n Socket creation error \n");
            return -1;
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
        }

        // Connect to the server
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            printf("\nConnection Failed \n");
            return -1;
        }

        // Send a request to the server
        send(sock, "Request", strlen("Request"), 0);
        // Read the server's response
        read(sock, buffer, BUFFER_SIZE);
        printf("Response from server (Client %d):\n%s\n", i + 1, buffer);

        close(sock);
    }

    return 0;
}
