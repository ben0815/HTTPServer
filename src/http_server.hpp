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

    http_server(std::string _port, std::string _root_dir);

    ~http_server();

    // Setup the addrinfo struct for this server.
    void initialize();

    // Startup the http server. Create a socket, bind the socket, listen at the
    // given port, accept connections, and receive requests.
    void run();

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
