#ifndef HTTP_SERVER_HPP_
#define HTTP_SERVER_HPP_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <iterator>
#include <algorithm>


class http_server {

  public:

    http_server() {}

    // Construct and initialize server. Setup address.
    http_server(std::string _port, std::string _root);

    ~http_server();

    // Startup the http server. Create a socket, bind the socket, listen at the
    // given port, accept connections, and receive requests.
    void run();

    // Parse incoming requests.
    void parse(const std::string& _request) const;

    // Handle and respond to GET requests.
    void handle_get_request(const std::string& _req_line) const;

    // Serve a valid resource.
    void serve(const std::string& _content_type, const std::string& _path) const;

    // Respond with an error code.
    void error_response(const std::string& _error) const;

    // Check if we can handle the requested file based on its extension.
    std::pair<bool, std::string> is_valid_ext(const std::string& _extension) const;

  private:

    // Main socket file descriptor.
    int m_sfd{-1};

    // Socket file descriptor which has accepted a connection.
    int m_accepted{-1};

    std::string m_port{"2490"};

    std::string m_root{"./"};

    addrinfo* m_addr{nullptr};
};


#endif
