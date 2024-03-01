#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include "client.hpp"

int main() {
  Client client(server_info);

  try {
    client.connect_to_server();
    client.send_messages();
  } catch (const ClientInitException& e) {
    std::cerr << "Error while initializing client: " << e.what() << '\n';
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
  }

  return 0;
}