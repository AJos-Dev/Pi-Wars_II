#include <iostream>

#include <cstdint>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

void *create_buffer(uint32_t offset = 16, uint32_t stride = 8,
                    uint32_t *data = nullptr) {
  uint32_t *buffer = new uint32_t[2 + stride];
  buffer[0] = offset;
  buffer[1] = stride;
  for (int i = 0; i < stride; i++) {
    buffer[2 + i] = data[i];
  }
  return buffer;
}

// this function unpacks the buffer and returns the data pointer.
uint32_t *unpack_buffer(void *buffer) {
  uint32_t *data = new uint32_t[2 + ((uint32_t *)buffer)[1]];
  for (int i = 0; i < 2 + ((uint32_t *)buffer)[1]; i++) {
    data[i] = ((uint32_t *)buffer)[i];
  }
  return data;
}

struct messageStuct {
  uint32_t offset;
  uint32_t stride;
  uint32_t *data;
};

bool sendData(messageStuct *message) {
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(8080);
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (connect(clientSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    std::cout << "Error connecting to server" << std::endl;
    return false;
  }
  send(clientSocket, message, sizeof(message), 0);
  close(clientSocket);

  return true;
}

bool recieveData(messageStuct *message) {
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(8080);
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    std::cout << "Error binding socket" << std::endl;
    return false;
  }
  if (listen(serverSocket, 1) < 0) {
    std::cout << "Error listening on socket" << std::endl;
    return false;
  }
  sockaddr_in clientAddr;
  socklen_t clientAddrSize = sizeof(clientAddr);
  int clientSocket =
      accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrSize);
  if (clientSocket < 0) {
    std::cout << "Error accepting client" << std::endl;
    return false;
  }
  recv(clientSocket, message, sizeof(message), 0);
  close(clientSocket);
  close(serverSocket);
  return true;
}

int main() {
  // create 720x480 pixel buffer, with each pixel described by a single uint32_t
  // value.
  uint32_t *data = new uint32_t[720 * 480];
  // the colour is red for all pixels.
  for (int i = 0; i < 720 * 480; i++) {
    data[i] = 0xFF0000FF;
  }

  // create the buffer
  void *buffer = create_buffer(16, 8, data);

  // send the buffer
  messageStuct message;
  message.offset = 16;
  message.stride = 8;
  message.data = data;

  sendData(&message);

  // messageStuct recievedMessage;
  // recieveData(&recievedMessage);

  // for (int i = 0; i < 10; i++) {
  //   std::cout << std::hex << recievedMessage.data[i] << std::endl;
  // }

  // // unpack the buffer
  // uint32_t *data2 = unpack_buffer(buffer);
  //
  // // print the first 10 values of the unpacked buffer
  // for (int i = 0; i < 10; i++) {
  //   std::cout << std::hex << data2[i] << std::endl;
  // }
}
