#ifndef HTTP_SERVER_HPP_
#define HTTP_SERVER_HPP_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <iostream>


class http_server {

  public:

    http_server() {}

    // Construct and initialize server. Setup address.
    http_server(std::string _port, std::string _root_dir);

    ~http_server();

    // Startup the http server. Create a socket, bind the socket, listen at the
    // given port, accept connections, and receive requests.
    void run();

    // Parse incoming requests.
    void parse(const std::string& _request) const;

    // Handle and respond to GET requests.
    void handle_get_request(const std::string& _req_line) const;

    // Respond with an error code.
    void error_response(const std::string& _error) const;

  private:

    // Main socket file descriptor.
    int m_sfd{-1};

    // Socket file descriptor which has accepted a connection.
    int m_accepted{-1};

    std::string m_port{"2490"};

    std::string m_root_dir{"./"};

    addrinfo* m_addr{nullptr};
};


#endif
