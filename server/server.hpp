#ifndef SERVER_HPP
#define SERVER_HPP

#include "../shared/exceptions.hpp"
#include "../shared/server_info.hpp"
#include "quicksort_utils.hpp"
#include <arpa/inet.h>
#include <cstddef>
#include <exception>
#include <iostream>
#include <iterator>
#include <netdb.h>
#include <sstream>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

class Server {
public:
  Server(const ServerInfo &server_info);
  Server(const Server &other);
  Server &operator=(const Server &other);
  ~Server();

  void start();

private:
  static constexpr std::size_t MAX_BUFFER_SIZE = 255;

  int listener;
  ServerInfo server_info;
  fd_set descriptors;
  int max_descriptor;
  void listen();
  void copy(const Server &other);
  void clear();
  void display_welcome(int client) const;
  void display_commands(int client,
                        std::stringstream &&stream = std::stringstream()) const;
  void parse_command(const std::string &command, int client);
  std::vector<std::string> split_input(const std::string &input) const;
  std::string read_request(int client);
};

#endif