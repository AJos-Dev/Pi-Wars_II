

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

// Please ensure that your offset and stride are in bytes NOT bits
// uint32_t *createBufferFromData(const uint32_t *data, size_t stride = 1,
//                                size_t offset = 3, size_t dataSize = 1024) {
//   uint32_t *buffer = new uint32_t[dataSize + offset];
//   buffer[0] = offset;
//   buffer[1] = stride;
//   buffer[2] = dataSize;
//   for (size_t i = offset; i < (dataSize + offset); i += stride) {
//     buffer[i] = data[i - offset];
//   }
//
//   return buffer;
// }
//
uint32_t *readBuffer(const uint32_t *buffer) {
  uint32_t offset = buffer[0];
  uint32_t stride = buffer[1];
  uint32_t dataSize = buffer[2];

  uint32_t *data = malloc(dataSize * sizeof(uint32_t));
  if (data == NULL) {
    // TODO: Handle memory allocation failure
    return NULL;
  }

  for (size_t i = 0; i < dataSize; i++) {
    data[i] = buffer[i + offset];
  }

  return data;
}

uint32_t *readBufferFromSocket(int socket_fd) {
  uint32_t header[3];

  // Read the header containing offset, stride, and datasize
  ssize_t bytes_received = recv(socket_fd, header, sizeof(header), 0);

  if (bytes_received == -1) {
    perror("Error while receiving data");
    return NULL;
  }

  if (bytes_received != sizeof(header)) {
    fprintf(stderr, "Incomplete header received. Received %zd bytes.\n",
            bytes_received);
    return NULL;
  }

  // Allocate memory for the complete buffer
  size_t buffer_size = header[2] + header[0];
  uint32_t *buffer = malloc(buffer_size * sizeof(uint32_t));

  if (buffer == NULL) {
    perror("Memory allocation failed");
    exit(EXIT_FAILURE);
  }

  // Copy the received header to the buffer
  memcpy(buffer, header, sizeof(header));

  // Read the rest of the buffer
  bytes_received =
      recv(socket_fd, buffer + 3, (buffer_size - 3) * sizeof(uint32_t), 0);

  if (bytes_received == -1) {
    perror("Error while receiving data");
    free(buffer);
    return NULL;
  }

  if (bytes_received != (buffer_size - 3) * sizeof(uint32_t)) {
    fprintf(stderr, "Incomplete buffer received. Received %zd bytes.\n",
            bytes_received);
    free(buffer);
    return NULL;
  }

  return buffer;
}

void send_buffer(uint32_t *buffer) {}

#define PORT 8080
#define SERVER_IP "127.0.0.1"

int main() {
  int client_socket;
  struct sockaddr_in server_addr;

  // Create socket
  client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket == -1) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  // Set up server address struct
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  inet_aton(SERVER_IP, &server_addr.sin_addr);

  // Connect to the server
  if (connect(client_socket, (struct sockaddr *)&server_addr,
              sizeof(server_addr)) == -1) {
    perror("Connection failed");
    close(client_socket); // Close the socket on connection failure
    exit(EXIT_FAILURE);
  }

  printf("Connected to server %s:%d\n", SERVER_IP, PORT);

  // Example buffer for receiving data
  uint32_t *received_buffer = readBufferFromSocket(client_socket);

  if (received_buffer != NULL) {
    // Process the received buffer (replace with your logic)
    for (size_t i = 0; i < received_buffer[2]; i++) {
      printf("received_buffer[%zu] = %u\n", i, received_buffer[i]);
    }

    // Free the received buffer
    free(received_buffer);
  } else {
    printf("Failed to receive buffer from the server.\n");
  }

  // Close client socket
  close(client_socket);

  return 0;
}
