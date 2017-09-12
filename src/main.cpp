#include <iostream>
#include <string>


#include "http_server.hpp"


int
main(int _argc, char** _argv) {
  std::string port = "2490";

  // Parse port number from command line arguments.
  if(_argc > 1)
    for(size_t i = 0; i < _argc; ++i) {
      const auto arg = std::string(_argv[i]);
      if(arg == "-p")
        port = _argv[i + 1];
    }
  else
    std::cerr << "Usage: ./server -p <port-number>" << std::endl
              << "Using default port (2490) since one was not provided."
              << std::endl;

  // Create an http server to begin receiving data from localhost at the given
  // port.
  http_server server(port);

  return 1;
}
