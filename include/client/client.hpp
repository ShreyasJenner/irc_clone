#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>

extern "C" {
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
}

#define MAX_CONN 1

class Client {
private:
  int sendingSockfd, listeningSockfd;
  int clientSocket;
  std::string ip_addr;
  std::string port;

public:
  Client(std::string ip_addr, std::string port);

  void start_client();

  void connect_to(std::string ip_addr, std::string port);

  void send_msg(std::string msg);

  std::string recv_msg();

  void close_sockets();

  ~Client();
};

#endif // !CLIENT_HPP
