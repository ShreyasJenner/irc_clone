#include "client/client.hpp"

// Parametric constructor to store client ip address and port
Client::Client(std::string ip_addr, std::string port) {
  this->ip_addr = ip_addr;
  this->port = port;
}

void Client::start_client() {
  int err, reuse;
  struct addrinfo hints, *res;

  // load address structs
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // populate address struct
  err = getaddrinfo(this->ip_addr.c_str(), this->port.c_str(), &hints, &res);
  if (err != 0) {
    perror("Error filling address structs");
    this->close_sockets();
    exit(1);
  }

  // create a listening socket
  this->clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (this->clientSocket < 0) {
    perror("Error creating listening socket");
    this->close_sockets();
    exit(1);
  }

  // allow reusing of socket address
  reuse = 1;
  setsockopt(this->clientSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

  // bind the socket to the given port
  err = bind(this->clientSocket, res->ai_addr, res->ai_addrlen);
  if (err != 0) {
    perror("Error binding socket to port");
    this->close_sockets();
    exit(1);
  }

  // start listening on the socket
  listen(this->clientSocket, MAX_CONN);
}

// Function to connect to a system with the given ip address over the given port
// and accept a connection from the server in return
void Client::connect_to(std::string ip_addr, std::string port) {
  struct sockaddr_storage other_addr;
  socklen_t addr_size;

  struct addrinfo hints, *res;
  int err, reuse;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  // get address info of server
  err = getaddrinfo(ip_addr.c_str(), port.c_str(), &hints, &res);
  if (err != 0) {
    perror("Error populating address struct");
    this->close_sockets();
    exit(1);
  }

  // make a socket and connect it to server
  this->sendingSockfd =
      socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (this->sendingSockfd == -1) {
    perror("Error creating socket");
    this->close_sockets();
    exit(1);
  }

  // allow reusing of socket
  reuse = 1;
  setsockopt(this->sendingSockfd, SOL_SOCKET, SO_REUSEADDR, &reuse,
             sizeof(int));

  // connect socket to server
  err = connect(sendingSockfd, res->ai_addr, res->ai_addrlen);
  if (err != 0) {
    perror("Error connecting to socket");
    this->close_sockets();
    exit(1);
  }

  // send address and port to server
  this->send_msg(this->ip_addr);
  this->send_msg(this->port);

  // accept a connection from the server
  addr_size = sizeof other_addr;
  this->listeningSockfd =
      accept(this->clientSocket, (struct sockaddr *)&other_addr, &addr_size);
  if (this->listeningSockfd == -1) {
    perror("Error accepting connection to socket");
    this->close_sockets();
    exit(1);
  }

  // allow reusing of listening socket
  reuse = 1;
  setsockopt(this->listeningSockfd, SOL_SOCKET, SO_REUSEADDR, &reuse,
             sizeof(int));
}

// Function to send a message to the server
void Client::send_msg(std::string msg) {
  int len;

  // Send the length of the message before sending the message
  len = msg.length();
  send(this->sendingSockfd, &len, sizeof(len), 0);
  send(this->sendingSockfd, msg.c_str(), msg.length(), 0);
}

// Function to receive a message from the server
std::string Client::recv_msg() {
  int len, err;
  std::string msg;

  // receive message length
  len = 0;
  err = recv(this->listeningSockfd, &len, sizeof(len), 0);
  if (err != sizeof(int)) {
    std::cout << err << " " << len << '\n';
    perror("Error receiving message length");
    this->close_sockets();
    exit(1);
  }
  char buff[len + 1];

  // receive message and add \0 at end
  err = recv(this->listeningSockfd, buff, len, 0);
  if (err != len) {
    perror("Error receiving message");
    this->close_sockets();
    exit(1);
  }
  buff[len] = '\0';

  msg = buff;
  return msg;
}

// Function to close sockets safely
void Client::close_sockets() {
  // shutdown all sockets
  shutdown(this->clientSocket, SHUT_RDWR);
  shutdown(this->sendingSockfd, SHUT_RDWR);
  shutdown(this->listeningSockfd, SHUT_RDWR);

  // close the sockets
  close(this->clientSocket);
  close(this->sendingSockfd);
  close(this->listeningSockfd);
}

// Destructor to free all allocated data
Client::~Client() { this->close_sockets(); }
