#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "../shared/exceptions.hpp"
#include "../shared/server_info.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

class Client {
public:
  Client(const ServerInfo &server_info);
  Client(const Client &other);
  Client &operator=(const Client &other);
  ~Client();

  void connect_to_server();
  void send_messages() const;

private:
  static constexpr std::size_t MAX_BUFFER_SIZE = 255;

  int client_socket;
  ServerInfo server_info;

  void show_response() const;
};

#endif