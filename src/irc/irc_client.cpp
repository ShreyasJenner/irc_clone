#include "irc/irc_client.hpp"

// Parametric constructor to store irc client ip address, username and port
IRC_Client::IRC_Client(std::string username, std::string ip_addr,
                       std::string port) {

  // initialize the class variables
  this->username = username;
  this->channels.push_back("common");

  // create the node client and store the details
  this->client = new Node(CLIENT, ip_addr, port);
}

// Function to connect to IRC server and send necessary details
void IRC_Client::init_connect(std::string ip_addr, std::string port) {
  // connect to server with given ip address and port and store socket obtained
  this->commSock = this->client->connect_to(ip_addr, port);

  // send channel, username, ip address and port to server
  this->client->send_msg(this->commSock, "common");
  this->client->send_msg(this->commSock, this->username);
  this->client->send_msg(this->commSock, this->client->get_ip_addr());
  this->client->send_msg(this->commSock, this->client->get_port());
}

// Function to handle sending of messages
void IRC_Client::handle_send() {
  std::string msg;

  // send messages as long as IRC client is running
  while (this->run) {
    // take msg from stdin
    std::getline(std::cin, msg);

    // send the channel, username, and message
    this->client->send_msg(this->commSock, *this->channels.begin());
    this->client->send_msg(this->commSock, this->username);
    this->client->send_msg(this->commSock, msg);
    if (msg == "STOP") {
      this->run = false;
    }
  }
}

// Function to handle receiving of messages
// TODO: add identifiers for finding who is sending the message
void IRC_Client::handle_recv() {
  std::string msg;

  // send messages as long as IRC client is running
  while (this->run) {
    msg = this->client->recv_msg(this->commSock);
    std::cout << "Received: " << msg << '\n';
    if (msg == "STOP") {
      this->run = false;
    }
  }
}

// Function to run irc client
void IRC_Client::run_IRC_Client(std::string ip_addr, std::string port) {
  // initialize connection to server
  this->init_connect(ip_addr, port);

  std::cout << "Running IRC Client\n";

  // thread to handle sending and receiving of messages
  std::thread t1(&IRC_Client::handle_send, this);
  std::thread t2(&IRC_Client::handle_recv, this);

  // wait till thread is complete
  t1.join();
  t2.join();
}

// Destructor to clean up resources
IRC_Client::~IRC_Client() {
  std::cout << "Closing IRC Client\n";

  // shutdown and close the communication socket
  this->client->close_sockets(this->commSock);

  // remove the node
  delete this->client;
}
