#ifndef NODE_HPP
#define NODE_HPP

#include <iostream>
#include <string>

extern "C" {
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
}

#define SERVER_MAX_CONN 10
#define CLIENT_MAX_CONN 1

enum NodeType { SERVER, CLIENT };

class Node {
private:
  int listenSock;
  NodeType type;

  struct pollfd listenPoll;

  std::string ip_addr;
  std::string port;

public:
  Node(NodeType type, std::string ip_addr, std::string port);

  std::string get_ip_addr();

  std::string get_port();

  void start_node();

  int accept_conns();

  int connect_to(std::string ip_addr, std::string port);

  void send_msg(int socket, std::string msg);

  std::string recv_msg(int socket);

  void close_sockets(int sock);

  ~Node();
};
#endif // !NODE_HPP
