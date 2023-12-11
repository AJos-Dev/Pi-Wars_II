#include <arpa/inet.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define IP "192.168.1.34"

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

int main(int argc, char *argv[]) {
  uint32_t data[720 * 480];
  for (int i = 0; i < 720 * 480; i++) {
    data[i] = 0xff0000ff;
  }

  uint32_t *buffer = create_buffer_from_data(data, 1, 3, 720 * 480);

  int opt = 1;
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
             sizeof(opt));

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(PORT);
  address.sin_addr.s_addr = inet_addr(IP);

  if (connect(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("Connection failed");
    return -1;
  }

  sendBuffer(socket_fd, buffer, 720 * 480 + (3 * 4));

  free(buffer);
  return 0;
}
