#ifndef SERVER_INFO_HPP
#define SERVER_INFO_HPP

#include <string>

struct ServerInfo {
  std::string hostname;
  std::string port;

  ServerInfo(const std::string &hostname, const std::string &port)
      : hostname(hostname), port(port) {}
};

static ServerInfo server_info("127.0.0.1", "3000");

#endif