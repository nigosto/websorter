#include "server.hpp"

int main() {
  Server server(server_info);

  try {
    server.start();
  } catch (const ServerStartupException &e) {
    std::cerr << "Error while starting server: " << e.what() << '\n';
  } catch (const AcceptConnectionException &e) {
    std::cerr << "Error while accepting connection: " << e.what() << '\n';
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << '\n';
  }

  return 0;
}