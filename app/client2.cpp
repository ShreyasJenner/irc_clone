#include "irc/irc_client.hpp"

int main() {
  IRC_Client irc_c("user2", "localhost", "1235");

  irc_c.run_IRC_Client("localhost", "8080");

  return 0;
}
