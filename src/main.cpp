#include <iostream>
#include <string>
#include <exception>


#include "http_server.hpp"


int
main(int _argc, char** _argv) {
  std::string port = "2490";
  std::string root_dir = "./";

  try {
    // Parse port number and root directory from command line arguments.
    if(_argc == 3) {
      port = std::string(_argv[1]);
      root_dir = std::string(_argv[2]);
    }
    else
      throw std::runtime_error("Usage: ./server <port-number> <document-root>");

    // Create an http server to begin receiving data from localhost at the given
    // port.
    http_server server(port, root_dir);
  }
  catch(std::exception const& _e) {
    std::cerr << "Runtime exception: " << _e.what() << std::endl;
  }

  return 1;
}
