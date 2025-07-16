#ifndef IRC_SERVER_HPP
#define IRC_SERVER_HPP

#include <iostream>
#include <thread>
#include <vector>

#include "node/node.hpp"

extern "C" {
#include <poll.h>
}

#define CMD_SYM '/'

class Users {
public:
  std::vector<std::string> channels;
  std::string username;
  std::string ip_addr;
  std::string port;
  int commSock;
  struct pollfd *poll_ptr;
};

class IRC_Server {
private:
  bool run;

  Node *server;

  std::vector<Users> user_data;
  std::vector<struct pollfd> pfd_vec;

public:
  IRC_Server(std::string ip_addr, std::string port);

  std::vector<Users> *get_user_data();

  void handle_connections();

  void handle_recv();

  void handle_send();

  void handle_client_exit(int index);

  void run_IRC_Server();

  ~IRC_Server();
};

#endif // !IRC_SERVER_HPP
