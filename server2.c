#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <dirent.h>
#include <errno.h>
#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// Function to get the top 2 CPU-consuming processes
void get_top_cpu_processes(char *buffer) {
    DIR *dir;
    struct dirent *entry;
    char path[BUFFER_SIZE];
    int pid;
    unsigned long utime, stime;
    char comm[256];
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
                fscanf(fp, "%d %s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu",
                       &pid, comm, &utime, &stime);

                // Add the process info to the result
                snprintf(result + strlen(result), BUFFER_SIZE - strlen(result),
                         "Process Name: %s, PID: %d, user: %lu, kernel: %lu\n",
                         comm, pid, utime, stime);

                fclose(fp);
                count++;
            }
        }
    }

    closedir(dir);
    strcpy(buffer, result);
}

int main() {
    int server_fd, client_sock[MAX_CLIENTS], max_sd, activity, sd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    fd_set readfds;
    char buffer[BUFFER_SIZE];

    // Initialize client socket array
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sock[i] = 0;
    }

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
        // Clear the socket set
        FD_ZERO(&readfds);
        // Add server socket to set
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // Add child sockets to set
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sock[i];
            // If valid socket descriptor then add to read list
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            // Keep track of the highest file descriptor
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // Wait for activity on one of the sockets
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        // If something happened on the master socket, then it's an incoming connection
        if (FD_ISSET(server_fd, &readfds)) {
            addr_size = sizeof(client_addr);
            int new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_size);
            if (new_socket < 0) {
                perror("Accept failed");
                continue;
            }

            printf("New connection accepted\n");

            // Add new socket to array of sockets
            for (int i = 0; i < MAX_CLIENTS; i++) {
                // If position is empty
                if (client_sock[i] == 0) {
                    client_sock[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
        }

        // Check for IO operations on existing sockets
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sock[i];
            if (FD_ISSET(sd, &readfds)) {
                // Check if it was for closing, and also read the incoming message
                int valread = read(sd, buffer, BUFFER_SIZE);
                if (valread == 0) {
                    // Someone disconnected, get details and print
                    getpeername(sd, (struct sockaddr *)&client_addr, (socklen_t *)&addr_size);
                    // Close the socket and mark as 0 in the list for reuse
                    close(sd);
                    client_sock[i] = 0;
                } else {
                    // Process incoming request from client
                    memset(buffer, 0, sizeof(buffer));
                    get_top_cpu_processes(buffer);
                    send(sd, buffer, strlen(buffer), 0);
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
