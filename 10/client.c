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
    int client_socket_fd;
    struct sockaddr_in client_address;
    struct ip_mreq multicast_request; 
    char message_buffer[MAX_BUFFER_SIZE];

    client_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket_fd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    int reuse_addr = 1;
    if (setsockopt(client_socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) < 0) {
        perror("Error setting SO_REUSEADDR option");
        close(client_socket_fd);
        exit(EXIT_FAILURE);
    }

    memset(&client_address, 0, sizeof(client_address));
    client_address.sin_family = AF_INET; 
    client_address.sin_port = htons(MULTICAST_PORT); 
    client_address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(client_socket_fd, (struct sockaddr *)&client_address, sizeof(client_address)) < 0) {
        perror("Error binding socket");
        close(client_socket_fd);
        exit(EXIT_FAILURE);
    }

    multicast_request.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDRESS); 
    multicast_request.imr_interface.s_addr = htonl(INADDR_ANY); 

    if (setsockopt(client_socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast_request, sizeof(multicast_request)) < 0) {
        perror("Error joining multicast group");
        close(client_socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Client for multicast started, joined group %s on port %d\n", MULTICAST_ADDRESS, MULTICAST_PORT);

    while (1) {
        ssize_t bytes_received = recvfrom(client_socket_fd, message_buffer, MAX_BUFFER_SIZE, 0, NULL, NULL);
        if (bytes_received < 0) {
            perror("Error receiving message");
        } else {
            message_buffer[bytes_received] = '\0';
            printf("Received: %s", message_buffer);
        }
    }

    close(client_socket_fd);

    printf("Client shutting down.\n");

    return 0;
}
