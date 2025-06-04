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

#define CLIENT2_TYPE "client2"
#define TERMINATION_MESSAGE "The End"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <SERVER_IP> <SERVER_PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *server_ip_address = argv[SERVER_IP_INDEX];
    int server_port_number = atoi(argv[SERVER_PORT_INDEX]);

    int client_socket_fd;
    struct sockaddr_in server_address;

    client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket_fd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port_number);

    if (inet_pton(AF_INET, server_ip_address, &server_address.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(client_socket_fd);
        exit(EXIT_FAILURE);
    }

    if (connect(client_socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error connecting to server");
        close(client_socket_fd);
        exit(EXIT_FAILURE);
    }

    if (send(client_socket_fd, CLIENT2_TYPE, strlen(CLIENT2_TYPE), 0) < 0) {
        perror("Error sending client type");
        close(client_socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server as %s. Waiting for messages...\n", CLIENT2_TYPE);

    char message_buffer[MAX_BUFFER_SIZE];
    ssize_t bytes_received;

    while ((bytes_received = recv(client_socket_fd, message_buffer, MAX_BUFFER_SIZE, 0)) > 0) {
        message_buffer[bytes_received] = '\0';
        printf("Received: %s", message_buffer);

        if (strstr(message_buffer, TERMINATION_MESSAGE) != NULL) {
            printf("Termination message \"%s\" received. Exiting.\n", TERMINATION_MESSAGE);
            break;
        }
    }

    if (bytes_received == 0) {
        printf("Server closed the connection.\n");
    } else {
        perror("Error receiving message from server");
    }

    close(client_socket_fd);

    printf("Client2 shutting down.\n");

    return 0;
}