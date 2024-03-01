#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>
#include <string>

class ClientInitException : public std::exception {
private:
  std::string message;

public:
  ClientInitException(const std::string &message) : message(message) {}

  const char *what() const noexcept final { return message.c_str(); }
};

class ServerStartupException : public std::exception {
private:
  std::string message;

public:
  ServerStartupException(const std::string &message) : message(message) {}

  const char *what() const noexcept final { return message.c_str(); }
};

class AcceptConnectionException : public std::exception {
private:
  std::string message;

public:
  AcceptConnectionException(const std::string &message) : message(message) {}

  const char *what() const noexcept final { return message.c_str(); }
};

class ReceiveMessageException : public std::exception {
private:
  std::string message;

public:
  ReceiveMessageException(const std::string &message) : message(message) {}

  const char *what() const noexcept final { return message.c_str(); }
};

#endif