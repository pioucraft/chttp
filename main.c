#include <string.h>             // For strlen(), memset(), etc.
#include <unistd.h>             // For close()
#include <stdio.h>
#include <arpa/inet.h>          // For sockaddr_in, htons(), INADDR_ANY

#define PORT 8080
#define BUFFER_SIZE 4096

int main() {
    int server_fd, client_fd;   // File descriptors for server and client sockets
    struct sockaddr_in addr;    // Structure for server address info
    socklen_t addr_len = sizeof(addr); // Size of the address structure
    char buffer[BUFFER_SIZE];   // Buffer to hold incoming data

    /*
     * socket()
     * Creates a socket that will be used for communication.
     * Parameters:
     *  AF_INET     - IPv4 address family
     *  SOCK_STREAM - Stream socket (TCP)
     *  0           - Default protocol (TCP)
     * Returns:
     *  Socket file descriptor (>= 0) on success, or -1 on failure.
     */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error creating socket");
        return 1;
    }

    // Initialize the sockaddr_in struct to zero to avoid garbage values
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;                // Use IPv4
    addr.sin_addr.s_addr = INADDR_ANY;        // Listen on all available interfaces
    /*
     * htons()
     * Converts the port number from host byte order to network byte order (big-endian).
     * This ensures that port is represented correctly on the network.
     */
    addr.sin_port = htons(PORT);

    /*
     * bind()
     * Associates the socket with the specified IP address and port.
     * Parameters:
     *  server_fd               - Socket file descriptor created earlier
     *  (struct sockaddr *)&addr - Pointer to address structure (cast to generic sockaddr)
     *  sizeof(addr)            - Size of the address structure
     * Returns:
     *  0 on success, -1 on failure.
     */
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Error binding socket");
        close(server_fd);
        return 1;
    }

    /*
     * listen()
     * Marks the socket as passive and ready to accept incoming connections.
     * Parameters:
     *  server_fd - Socket file descriptor
     *  500       - Maximum number of pending connections in the queue
     * Returns:
     *  0 on success, -1 on failure.
     */
    if (listen(server_fd, 500) < 0) {
        perror("Error listening on socket");
        close(server_fd);
        return 1;
    }

    printf("THE GOATED C SERVER IS LISTENING ON PORT %d\n", PORT);
    printf("THE GOATED C SERVER IS READY TO ACCEPT CONNECTIONS\n");
    printf("THE GOATED C SERVER IS A PRODUCT OF GOUGOULE INC.\n");

    while (1) {
        /*
         * accept()
         * Waits for an incoming client connection.
         * Parameters:
         *  server_fd        - Listening socket file descriptor
         *  (struct sockaddr *)&addr - Pointer to sockaddr structure to store client address
         *  &addr_len        - Pointer to socklen_t containing size of addr
         * Returns:
         *  New socket file descriptor for the accepted client connection, or -1 on error.
         */
        client_fd = accept(server_fd, (struct sockaddr *)&addr, &addr_len);
        if (client_fd < 0) {
            perror("Error accepting connection");
            continue;
        }
        pid_t pid = fork(); // Create a new process to handle the client
        if (pid < 0) {
            perror("Error forking process");
            close(client_fd);
            continue;
        } else if (pid > 0) {
            // Parent process: close the client socket and continue to accept new connections
            close(client_fd);
            continue;
        }

        /*
         * recv()
         * Reads incoming data from the connected client socket.
         * Parameters:
         *  client_fd    - Connected client socket file descriptor
         *  buffer      - Buffer to store received data
         *  BUFFER_SIZE - Max bytes to read
         *  0           - Flags (none)
         * Returns:
         *  Number of bytes received, 0 if connection closed, or -1 on error.
         */
        int bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received < 0) {
            perror("Error receiving data");
            close(client_fd);
            continue;
        }

        // Null-terminate the received data to safely print as string
        buffer[bytes_received] = '\0';

        /*
         * send()
         * Sends the HTTP response string back to the client.
         * Parameters:
         *  client_fd - Connected client socket descriptor
         *  response  - Pointer to response data
         *  strlen(response) - Length of response data
         *  0         - Flags (none)
         * Returns:
         *  Number of bytes sent, or -1 on error.
         */
        const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, World!";
        send(client_fd, response, strlen(response), 0);

        /*
         * close()
         * Closes the client socket, ending the connection.
         * Parameters:
         *  client_fd - Socket file descriptor to close.
         * Returns:
         *  0 on success, -1 on error.
         */
        close(client_fd);
    }

    // Close the server socket (never reached due to infinite loop)
    close(server_fd);

    return 0;
}
