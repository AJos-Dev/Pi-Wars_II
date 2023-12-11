#include <arpa/inet.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

// TODO: Confirm memoru allocation is correct, especially for the for loop.

uint32_t *create_buffer_from_data(const uint32_t *data, size_t stride,
                                  size_t offset, size_t datasize) {
  uint32_t *buffer = malloc((datasize + offset) * sizeof(uint32_t));
  if (buffer == NULL) {
    // Handle memory allocation failure
    return NULL;
  }

  buffer[0] = (uint32_t)offset;
  buffer[1] = (uint32_t)stride;
  buffer[2] = (uint32_t)datasize;

  for (size_t i = 0; i < datasize; i++) {
    buffer[i + offset] = data[i];
  }

  return buffer;
}

int sendBuffer(int socket_fd, const uint32_t *buffer, size_t buffer_size) {
  // Check for a valid socket file descriptor
  if (socket_fd < 0) {
    perror("Invalid socket file descriptor");
    return -1;
  }

  // Attempt to send the buffer over the socket
  ssize_t bytes_sent =
      send(socket_fd, buffer, buffer_size * sizeof(uint32_t), 0);

  // Check for errors during the send operation
  if (bytes_sent == -1) {
    perror("Error while sending data");
    return -1;
  }

  // Check if all bytes were sent
  if (bytes_sent != buffer_size * sizeof(uint32_t)) {
    fprintf(stderr, "Incomplete send. Sent %zd bytes out of %zu.\n", bytes_sent,
            buffer_size * sizeof(uint32_t));
    return -1;
  }

  return 0; // Success
}

int main() {
  uint32_t data[720 * 480];
  for (int i = 0; i < 720 * 480; i++) {
    data[i] = 0xff0000ff;
  }
  int server_socket, new_socket;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size = sizeof(struct sockaddr_in);

  // Create socket
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  // Set up server address struct
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Bind the socket
  if (bind(server_socket, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) == -1) {
    perror("Binding failed");
    exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  if (listen(server_socket, 10) == -1) {
    perror("Listening failed");
    exit(EXIT_FAILURE);
  }

  printf("Server listening on port %d...\n", PORT);

  // Accept a connection
  new_socket =
      accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
  if (new_socket == -1) {
    perror("Acceptance failed");
    exit(EXIT_FAILURE);
  }

  printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr),
         ntohs(client_addr.sin_port));

  uint32_t *buffer = create_buffer_from_data(data, 1, 3, 720 * 480);

  // Send buffer
  if (sendBuffer(new_socket, buffer, buffer[2]) == 0) {
    printf("Buffer sent successfully!\n");
  } else {
    printf("Failed to send buffer.\n");
  }

  // Close new socket
  close(new_socket);

  // Close server socket
  close(server_socket);

  // Free buffer memory
  free(buffer);

  return 0;
}
