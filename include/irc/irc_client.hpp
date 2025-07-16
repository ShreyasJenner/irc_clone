#ifndef IRC_CLIENT_HPP
#define IRC_CLIENT_HPP

#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "node/node.hpp"

class IRC_Client {
private:
  bool run;
  std::string username;
  int commSock;

  std::vector<std::string> channels;

  Node *client;

public:
  IRC_Client(std::string username, std::string ip_addr, std::string port);

  void init_connect(std::string ip_addr, std::string port);

  void handle_send();

  void handle_recv();

  void run_IRC_Client(std::string ip_addr, std::string port);

  ~IRC_Client();
};

#endif // !IRC_CLIENT_HPP
