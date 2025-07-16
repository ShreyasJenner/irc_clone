#include "node/node.hpp"

// Parametric constructor to store node ip address and port
Node::Node(NodeType type, std::string ip_addr, std::string port) {
  this->type = type;
  this->ip_addr = ip_addr;
  this->port = port;
}

// Getter function to get ip address of node
std::string Node::get_ip_addr() { return this->ip_addr; }

// Getter function to get port of node
std::string Node::get_port() { return this->port; }

// Function to start the node
void Node::start_node() {
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
    this->close_sockets(this->listenSock);
    exit(1);
  }

  // create a listening socket
  this->listenSock = socket(AF_INET, SOCK_STREAM, 0);
  if (this->listenSock < 0) {
    perror("Error creating listening socket");
    this->close_sockets(this->listenSock);
    exit(1);
  }

  // allow reusing of socket address
  reuse = 1;
  setsockopt(this->listenSock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

  // bind the socket to the given port
  err = bind(this->listenSock, res->ai_addr, res->ai_addrlen);
  if (err != 0) {
    perror("Error binding socket to port");
    this->close_sockets(this->listenSock);
    exit(1);
  }

  // start listening on the socket
  err = listen(this->listenSock,
               (type == SERVER) ? SERVER_MAX_CONN : CLIENT_MAX_CONN);
  if (err != 0) {
    perror("Error listening on socket");
    this->close_sockets(this->listenSock);
    exit(1);
  }

  // add listening socket to poll struct and set it
  this->listenPoll.fd = this->listenSock;
  this->listenPoll.events = POLLIN;
}

// Function to accept connections on listening socket and return a socket for
// communication
// Polls the listening socket and returns the socket number if a connection has
// been accepted
// else returns -1
int Node::accept_conns() {
  int commSock, reuse;

  // poll listening socket
  if (poll(&this->listenPoll, 1, 2000) > 0) {
    // accept connection from client
    commSock = accept(this->listenSock, nullptr, nullptr);
    if (commSock == -1) {
      perror("Error accepting connection to socket");
      this->close_sockets(this->listenSock);
      return -1;
    }

    // allow reusing of socket
    reuse = 1;
    setsockopt(commSock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

    return commSock;
  }

  return -1;
}

// Function to connect to a system with the given ip address over the given port
// and return a communication socket
int Node::connect_to(std::string ip_addr, std::string port) {

  struct addrinfo hints, *res;
  int err, reuse;
  int commSock;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  // get address info of server
  err = getaddrinfo(ip_addr.c_str(), port.c_str(), &hints, &res);
  if (err != 0) {
    perror("Error populating address struct");
    this->close_sockets(this->listenSock);
    return -1;
  }

  // make a socket
  commSock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (commSock == -1) {
    perror("Error creating socket");
    this->close_sockets(this->listenSock);
    return -1;
  }

  // allow reusing of socket
  reuse = 1;
  setsockopt(commSock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

  // connect socket to server
  err = connect(commSock, res->ai_addr, res->ai_addrlen);
  if (err != 0) {
    perror("Error connecting to socket");
    this->close_sockets(this->listenSock);
    return -1;
  }

  return commSock;
}

// Function to send a message through the given socket
void Node::send_msg(int socket, std::string msg) {
  int len;

  // Send the length of the message before the sending the message
  len = msg.length();
  send(socket, &len, sizeof(len), 0);
  send(socket, msg.c_str(), msg.length(), 0);
}

// Function to receive a message through the given socket
std::string Node::recv_msg(int socket) {
  int len, err;
  std::string msg;

  // receive message length
  err = recv(socket, &len, sizeof(len), 0);
  if (err != sizeof(int)) {
    perror("Error receiving message length");
    this->close_sockets(this->listenSock);
  }
  char buff[len + 1];

  // receive message and add \0
  err = recv(socket, buff, len, 0);
  if (err != len) {
    perror("Error receiving message");
    this->close_sockets(this->listenSock);
  }
  buff[len] = '\0';

  msg = buff;
  return msg;
}

// Function to close the sockets safely
void Node::close_sockets(int sock) {
  shutdown(sock, SHUT_RDWR);
  close(sock);
}

// Destructor for node class
Node::~Node() { this->close_sockets(this->listenSock); }
