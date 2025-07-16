#include "irc/irc_server.hpp"

int main() {
  IRC_Server irc_s("localhost", "8080");

  irc_s.run_IRC_Server();

  return 0;
}
