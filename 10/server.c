#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MULTICAST_PORT 12345
#define MULTICAST_ADDRESS "239.0.0.1"
#define MAX_BUFFER_SIZE 1024

int main() {
    int server_socket_fd;
    struct sockaddr_in multicast_group_address;
    char message_buffer[MAX_BUFFER_SIZE];

    server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket_fd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    int loopback_disable = 0; // 0 to disable loopback
    if (setsockopt(server_socket_fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loopback_disable, sizeof(loopback_disable)) < 0) {
        perror("Error disabling multicast loopback");
        // This is not critical, so we can continue
    }

    memset(&multicast_group_address, 0, sizeof(multicast_group_address));
    multicast_group_address.sin_family = AF_INET;
    multicast_group_address.sin_port = htons(MULTICAST_PORT);

    if (inet_pton(AF_INET, MULTICAST_ADDRESS, &multicast_group_address.sin_addr) <= 0) {
        perror("Invalid multicast address");
        close(server_socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Server for multicast started, sending to %s:%d\n", MULTICAST_ADDRESS, MULTICAST_PORT);
    printf("Enter messages to multicast (Ctrl+D to exit):\n");

    while (fgets(message_buffer, MAX_BUFFER_SIZE, stdin) != NULL) {
        ssize_t bytes_sent = sendto(server_socket_fd, message_buffer, strlen(message_buffer), 0,
                                      (struct sockaddr *)&multicast_group_address, sizeof(multicast_group_address));
        if (bytes_sent < 0) {
            perror("Error sending multicast message");
        }
    }

    printf("\nServer shutting down.\n");

    close(server_socket_fd);

    return 0;
}
