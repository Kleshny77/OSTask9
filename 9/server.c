#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024
#define SERVER_PORT_INDEX 2
#define SERVER_IP_INDEX 1
#define LISTEN_BACKLOG 2 // Max number of pending connections

#define CLIENT1_TYPE "client1"
#define CLIENT2_TYPE "client2"
#define TERMINATION_MESSAGE "The End"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <SERVER_IP> <SERVER_PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *server_ip_address = argv[SERVER_IP_INDEX];
    int server_port_number = atoi(argv[SERVER_PORT_INDEX]);

    int listen_socket_fd;
    struct sockaddr_in server_address;

    listen_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket_fd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port_number);

    if (inet_pton(AF_INET, server_ip_address, &server_address.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(listen_socket_fd);
        exit(EXIT_FAILURE);
    }

    if (bind(listen_socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error binding socket");
        close(listen_socket_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(listen_socket_fd, LISTEN_BACKLOG) < 0) {
        perror("Error listening on socket");
        close(listen_socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on %s:%d\n", server_ip_address, server_port_number);

    int client1_socket_fd = -1;
    int client2_socket_fd = -1;
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    while (client1_socket_fd == -1 || client2_socket_fd == -1) {
        int client_connection_fd = accept(listen_socket_fd, (struct sockaddr *)&client_address, &client_address_len);
        if (client_connection_fd < 0) {
            perror("Error accepting client connection");
            continue;
        }

        char client_type[10];
        ssize_t bytes_received = recv(client_connection_fd, client_type, sizeof(client_type) - 1, 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                printf("Client disconnected prematurely.\n");
            } else {
                perror("Error receiving client type");
            }
            close(client_connection_fd);
            continue;
        }
        client_type[bytes_received] = '\0';

        if (strcmp(client_type, CLIENT1_TYPE) == 0) {
            if (client1_socket_fd == -1) {
                client1_socket_fd = client_connection_fd;
                printf("Client1 connected from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            } else {
                printf("Client1 already connected. Rejecting new connection.\n");
                close(client_connection_fd);
            }
        } else if (strcmp(client_type, CLIENT2_TYPE) == 0) {
            if (client2_socket_fd == -1) {
                client2_socket_fd = client_connection_fd;
                printf("Client2 connected from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            } else {
                printf("Client2 already connected. Rejecting new connection.\n");
                close(client_connection_fd);
            }
        } else {
            printf("Unknown client type received: %s. Closing connection.\n", client_type);
            close(client_connection_fd);
        }
    }

    printf("Both clients connected. Starting message relay.\n");

    char message_buffer[MAX_BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = recv(client1_socket_fd, message_buffer, MAX_BUFFER_SIZE, 0)) > 0) {
        message_buffer[bytes_read] = '\0';

        if (send(client2_socket_fd, message_buffer, bytes_read, 0) < 0) {
            perror("Error sending message to Client2");
            break;
        }

        printf("Relayed from Client1 to Client2: %s", message_buffer);

        if (strstr(message_buffer, TERMINATION_MESSAGE) != NULL) {
            printf("Termination message \"%s\" received. Shutting down.\n", TERMINATION_MESSAGE);
            break;
        }
    }

    if (client1_socket_fd != -1) close(client1_socket_fd);
    if (client2_socket_fd != -1) close(client2_socket_fd);
    close(listen_socket_fd);

    printf("Server shutting down.\n");

    return 0;
}