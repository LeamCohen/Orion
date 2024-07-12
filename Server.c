#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 9898
#define MAX_CLIENTS 5

void *handle_client(void *arg);

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    int addrlen = sizeof(server_addr);
    pthread_t tid;
    char buffer_pool [] = {};
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);
    // Accept incoming connections and handle each in a new thread
    while (1) {
        if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }
        printf("New connection accepted.\n");
        // Create a new thread for this client
        if (pthread_create(&tid, NULL, handle_client, (void *)&client_fd) != 0) {
            perror("Thread creation failed");
            close(client_fd);
            continue;
        }   
        // Detach the thread so it can free its resources when it finishes
        pthread_detach(tid);
    }
    return 0;
}

void *handle_client(void *arg) {
    int client_fd = *((int *)arg);
    char buffer[1024] = {0};
    ssize_t bytes_read;
    // Read from client
    while ((bytes_read = read(client_fd, buffer, sizeof(buffer))) > 0) {
        printf("Client says: %s", buffer);
        
        // Echo back to client
        if (write(client_fd, buffer, strlen(buffer)) != bytes_read) {
            perror("Write failed");
            break;
        }    
        memset(buffer, 0, sizeof(buffer));  // Clear buffer
    }
    if (bytes_read == 0) {
        printf("Client disconnected.\n");
    } else if (bytes_read < 0) {
        perror("Read failed");
    }
    close(client_fd);
    return NULL;
}
