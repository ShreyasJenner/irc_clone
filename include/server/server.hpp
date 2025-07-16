#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>

extern "C" {
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
}

#define MAX_CONN 10

class Users {
public:
  std::string ip_addr;
  int port;
  int listeningSocket;
  int sendingSocket;
};

class Server {
private:
  std::string ip_addr;
  std::string port;

  int serverSocket;

  std::vector<Users> user_data;

public:
  Server(std::string ip_addr, std::string port);

  void start_server();

  void accept_conns();

  void send_msg(int socket, std::string msg);

  std::string recv_msg(int socket);

  std::vector<Users> *get_user_data();

  void close_sockets();

  ~Server();
};

#endif // !SERVER_HPP
