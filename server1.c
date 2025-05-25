#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// Function to get the top 2 CPU-consuming processes
void get_top_cpu_processes(char *buffer) {
    DIR *dir;
    struct dirent *entry;
    char path[BUFFER_SIZE], stat_file[BUFFER_SIZE];
    int pid;
    unsigned long utime, stime;
    char comm[256], state;
    char result[BUFFER_SIZE];
    strcpy(result, "Top 2 CPU-consuming processes:\n");

    int count = 0;

    // Open /proc directory to find all processes
    if ((dir = opendir("/proc")) == NULL) {
        perror("Failed to open /proc");
        strcpy(buffer, "Error reading processes");
        return;
    }

    // Iterate through /proc directory entries
    while ((entry = readdir(dir)) != NULL && count < 2) {
        // Check if the directory name is a number (PID)
        if (entry->d_type == DT_DIR && atoi(entry->d_name) > 0) {
            snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name);
            FILE *fp = fopen(path, "r");

            if (fp != NULL) {
                // Read the necessary fields from /proc/[pid]/stat
                fscanf(fp, "%d %s %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", 
                       &pid, comm, &state, &utime, &stime);

                // Add the process info to the result
                snprintf(result + strlen(result), BUFFER_SIZE, 
                         "Process Name: %s, PID: %d,user: %lu, kernel: %lu\n",
                         comm, pid, utime, stime);

                fclose(fp);
                count++;
            }
        }
    }

    closedir(dir);
    strcpy(buffer, result);
}

// Function to handle each client
void *handle_client(void *client_socket) {
    int client_sock = *(int *)client_socket;
    free(client_socket);

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    // Get top CPU-consuming processes
    get_top_cpu_processes(buffer);

    // Send the result back to the client
    send(client_sock, buffer, strlen(buffer), 0);

    // Close the socket
    close(client_sock);
    return NULL;
}

int main() {
    int server_fd, client_sock, *new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    pthread_t tid;

    // Create the socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set server address to IPv4 and specify port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        addr_size = sizeof(struct sockaddr_in);
        client_sock = accept(server_fd, (struct sockaddr *)&client_addr, &addr_size);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        printf("New connection accepted\n");

        // Allocate memory for new socket and create thread
        new_sock = malloc(1);
        *new_sock = client_sock;
        if (pthread_create(&tid, NULL, handle_client, (void *)new_sock) != 0) {
            perror("Failed to create thread");
        }

        // Detach thread to avoid memory leak
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}
