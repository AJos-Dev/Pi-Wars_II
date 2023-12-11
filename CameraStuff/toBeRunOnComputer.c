

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
  bytes_received = recv(socket_fd, buffer + sizeof(header) / sizeof(uint32_t),
                        (buffer_size - 3) * sizeof(uint32_t), 0);

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

int main() {

  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  int opt = 1;
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
             sizeof(opt));

  // we are listening on port 8080
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(8080);
  address.sin_addr.s_addr = INADDR_ANY;

  if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address))) {
    perror("Error while binding");
    exit(EXIT_FAILURE);
  }
  listen(socket_fd, 3);

  uint32_t *buffer = readBufferFromSocket(socket_fd);

  uint32_t *data = readBuffer(buffer);

  // print the data
  //
  for (int i = 0; i < 10; i++) {
    printf("%d\n", data[i]);
  }

  free(buffer);
  free(data);
  return 0;
}
