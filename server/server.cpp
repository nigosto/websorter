#include "server.hpp"

void Server::display_welcome(int client) const {
  std::stringstream buffer;
  buffer << "__        __   _                          \n";
  buffer << "\\ \\      / /__| | ___ ___  _ __ ___   ___ \n";
  buffer << " \\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\\n";
  buffer << "  \\ V  V /  __/ | (_| (_) | | | | | |  __/\n";
  buffer << "   \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___|\n";

  display_commands(client, std::move(buffer));
}

void Server::display_commands(int client, std::stringstream &&stream) const {
  stream << "=============================================================\n";
  stream << "quicksort [ARGS]... - sorts provided elements using quicksort\n";
  stream << "help - displays all available commands\n";
  stream << "exit - closes connection to the server\n";
  stream << "=============================================================";

  std::string message = stream.str();

  write(client, message.c_str(), message.size());
}

void Server::parse_command(const std::string &command, int client) {
  if (command == "exit") {
    FD_CLR(client, &descriptors);
    close(client);
  } else if (command == "help") {
    display_commands(client);
  } else if (command.starts_with("quicksort ")) {
    std::string args = command.substr(9);
    std::vector<std::string> input = split_input(args);

    quicksort_utils::quicksort(input.begin().base(), input.end().base());

    std::string response;
    for (const std::string &element : input) {
      (response += element) += ' ';
    }

    write(client, response.c_str(), response.size());
  } else {
    write(client, "Unrecognized command", 20);
  }
}

std::vector<std::string> Server::split_input(const std::string &input) const {
  std::vector<std::string> result;
  std::string::size_type pos = input.find(' '), prev = 0;
  while (pos != std::string::npos) {
    if (prev < pos) {
      result.push_back(input.substr(prev, pos - prev));
    }
    prev = pos + 1;
    pos = input.find(' ', pos + 1);
  }

  if (prev < pos) {
    result.push_back(input.substr(prev, pos - prev));
  }

  return result;
}

std::string Server::read_request(int client) {
  std::string result;

  char buff[MAX_BUFFER_SIZE] = "";
  int count = read(client, buff, sizeof(buff) - 1);

  if (count == 0) {
    FD_CLR(client, &descriptors);
    close(client);
  } else if (count < 0) {
    FD_CLR(client, &descriptors);
    close(client);
    throw ReceiveMessageException("Could not receive from client.");
  }

  while (count == sizeof(buff) - 1) {
    buff[count] = '\0';
    result += buff;
    count = read(client, buff, sizeof(buff) - 1);
  }

  if (count > 0) {
    buff[count] = '\0';
    result += buff;
  }

  return result;
}

void Server::copy(const Server &other) {
  server_info = other.server_info;
  listener = dup(other.listener);

  FD_ZERO(&descriptors);
  for (int i = 0; i <= other.max_descriptor; ++i) {
    if (FD_ISSET(i, &other.descriptors)) {
      FD_SET(i, &descriptors);
    }
  }

  max_descriptor = other.max_descriptor;
}

void Server::clear() {
  for (int i = 0; i <= max_descriptor; ++i) {
    if (FD_ISSET(i, &descriptors)) {
      FD_CLR(i, &descriptors);
      close(i);
    }
  }

  FD_ZERO(&descriptors);
  close(listener);
}

void Server::listen() {
  while (true) {
    fd_set current_descriptors = descriptors;

    int count = select(max_descriptor + 1, &current_descriptors, nullptr,
                       nullptr, nullptr);

    if (count == -1) {
      throw AcceptConnectionException("Could not select file descriptors.");
    }

    try {
      for (int i = 0; i <= max_descriptor; ++i) {
        if (FD_ISSET(i, &current_descriptors)) {
          if (i == listener) {
            sockaddr client_address;
            socklen_t client_address_length = sizeof(client_address);
            int client =
                accept(listener, &client_address, &client_address_length);

            if (client == -1) {
              throw AcceptConnectionException(
                  "Could not accept client's connection.");
            }

            FD_SET(client, &descriptors);
            max_descriptor = std::max(max_descriptor, client);

            display_welcome(client);
          } else {
            std::string command = read_request(i);

            if (command.size()) {
              parse_command(command, i);
            }
          }
        }
      }
    } catch (const AcceptConnectionException &e) {
      std::cerr << "Error while accepting connection: " << e.what() << '\n';
    } catch (const ReceiveMessageException &e) {
      std::cerr << "Error while receiving message: " << e.what() << '\n';
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << '\n';
    }
  }
}

Server::Server(const ServerInfo &server_info)
    : server_info(server_info), max_descriptor(0) {
  FD_ZERO(&descriptors);
}

Server::Server(const Server &other) : server_info(other.server_info) {
  copy(other);
}

Server &Server::operator=(const Server &other) {
  if (this != &other) {
    clear();
    copy(other);
  }

  return *this;
}

Server::~Server() { clear(); }

void Server::start() {
  addrinfo hints{}, *server_address_info;

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(server_info.hostname.c_str(), server_info.port.c_str(),
                  &hints, &server_address_info) != 0) {
    freeaddrinfo(server_address_info);
    throw ServerStartupException("Could not get server address.");
  }

  listener =
      socket(server_address_info->ai_family, server_address_info->ai_socktype,
             server_address_info->ai_protocol);

  if (listener == -1) {
    freeaddrinfo(server_address_info);
    throw ServerStartupException("Could not start socket.");
  }

  if (bind(listener, server_address_info->ai_addr,
           server_address_info->ai_addrlen) == -1) {
    freeaddrinfo(server_address_info);
    throw ServerStartupException("Could not assign server address to socket.");
  }

  freeaddrinfo(server_address_info);

  if (::listen(listener, SOMAXCONN) == -1) {
    throw ServerStartupException(
        "Could not make socket listen for connections.");
  }

  FD_SET(listener, &descriptors);
  max_descriptor = listener;

  listen();
}