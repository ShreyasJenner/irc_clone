#include "server/server.hpp"

// Parametric constructor to store ip address and port of the server
Server::Server(std::string ip_addr, std::string port) {
  this->ip_addr = ip_addr;
  this->port = port;
}

// Function to start the server and listen on a port
void Server::start_server() {

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
  this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (this->serverSocket < 0) {
    perror("Error creating listening socket");
    this->close_sockets();
    exit(1);
  }

  // allow reusing of socket address
  reuse = 1;
  setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

  // bind the socket to the given port
  err = bind(this->serverSocket, res->ai_addr, res->ai_addrlen);
  if (err != 0) {
    perror("Error binding socket to port");
    this->close_sockets();
    exit(1);
  }

  // start listening on the socket
  listen(this->serverSocket, MAX_CONN);
}

// Function to accept connections from clients
void Server::accept_conns() {
  struct sockaddr_storage other_addr;
  struct sockaddr_in *ptr;
  socklen_t addr_size;
  struct addrinfo hints, *res;

  int listenSock, sendSock, err;
  std::string ip_addr, port;

  // accept connection from client
  addr_size = sizeof other_addr;
  listenSock =
      accept(this->serverSocket, (struct sockaddr *)&other_addr, &addr_size);
  if (listenSock == -1) {
    perror("Error accepting connection to socket");
    this->close_sockets();
    exit(1);
  }

  // receive client ip and port
  ip_addr = this->recv_msg(listenSock);
  port = this->recv_msg(listenSock);

  // prepare the address info struct
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  // create a socket to send messages to the clients
  err = getaddrinfo(ip_addr.c_str(), port.c_str(), &hints, &res);
  if (err != 0) {
    perror("Error populating address struct");
    this->close_sockets();
    exit(1);
  }

  // make a socket and connect it to the client
  sendSock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (sendSock == -1) {
    perror("Error creating socket");
    this->close_sockets();
    exit(1);
  }

  err = connect(sendSock, res->ai_addr, res->ai_addrlen);
  if (err != 0) {
    perror("Error connecting to socket");
    this->close_sockets();
    exit(1);
  }

  // create an object of users class and populate it
  Users u;
  ptr = (struct sockaddr_in *)&other_addr;
  u.port = ptr->sin_port;
  u.ip_addr = ptr->sin_addr.s_addr;
  u.listeningSocket = listenSock;
  u.sendingSocket = sendSock;

  // push the user object into the vector
  this->user_data.push_back(u);
}

// Function to send a message to the client
void Server::send_msg(int socket, std::string msg) {
  int len;

  // Send the length of the message before sending the message
  len = msg.length();
  send(socket, &len, sizeof(len), 0);
  send(socket, msg.c_str(), msg.length(), 0);
}

// Function to receive a message from the server
std::string Server::recv_msg(int socket) {
  int len, err;
  std::string msg;

  // receive message length
  err = recv(socket, &len, sizeof(len), 0);
  if (err != sizeof(int)) {
    perror("Error receiving message length");
    this->close_sockets();
  }
  char buff[len + 1];

  // receive message and add \0
  err = recv(socket, buff, len, 0);
  if (err != len) {
    perror("Error receiving message");
    this->close_sockets();
  }
  buff[len] = '\0';

  msg = buff;
  return msg;
}

// Function to return pointer to vector of Users
std::vector<Users> *Server::get_user_data() { return &this->user_data; }

// Function to close sockets safely
void Server::close_sockets() {

  // close server socket
  shutdown(this->serverSocket, SHUT_RDWR);
  close(this->serverSocket);

  // close user socket
  for (auto user : this->user_data) {
    shutdown(user.sendingSocket, SHUT_RDWR);
    close(user.sendingSocket);

    shutdown(user.listeningSocket, SHUT_RDWR);
    close(user.listeningSocket);
  }
}

// Destructor to free all allocated data
Server::~Server() { this->close_sockets(); }
