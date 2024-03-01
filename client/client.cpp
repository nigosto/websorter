#include "client.hpp"

void Client::show_response() const {
  char buff[MAX_BUFFER_SIZE] = "";
  int count = read(client_socket, buff, sizeof(buff) - 1);
  if (count == 0) {
    std::cout << "Server closed connection\n";
    exit(0);
  }

  while (count == sizeof(buff) - 1) {
    buff[count] = '\0';
    std::cout << buff;
    count = read(client_socket, buff, sizeof(buff) - 1);
  }

  if (count > 0) {
    buff[count] = '\0';
    std::cout << buff;
  }

  std::cout << '\n';
}

Client::Client(const ServerInfo &server_info) : server_info(server_info) {}

Client::Client(const Client &other) : server_info(other.server_info) {
  client_socket = dup(other.client_socket);
}

Client &Client::operator=(const Client &other) {
  if (this != &other) {
    server_info = other.server_info;
    close(client_socket);
    client_socket = dup(other.client_socket);
  }

  return *this;
}

Client::~Client() { close(client_socket); }

void Client::connect_to_server() {
  addrinfo hints{}, *server_address_info;

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(server_info.hostname.c_str(), server_info.port.c_str(),
                  &hints, &server_address_info) != 0) {
    throw ClientInitException("Could not get server address.");
  }

  client_socket =
      socket(server_address_info->ai_family, server_address_info->ai_socktype,
             server_address_info->ai_protocol);

  if (client_socket == -1) {
    throw ClientInitException("Could not start socket.");
  }

  sockaddr server_address = *server_address_info->ai_addr;
  socklen_t server_address_length = sizeof(server_address);

  if (connect(client_socket, &server_address, server_address_length) == -1) {
    throw ClientInitException("Could not connect to server.");
  }

  freeaddrinfo(server_address_info);

  show_response();
}

void Client::send_messages() const {
  while (true) {
    std::cout << "> ";
    std::string buffer;
    std::getline(std::cin, buffer);

    write(client_socket, buffer.c_str(), buffer.size());

    show_response();
  }
}