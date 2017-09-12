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

    http_server(std::string _port);

    ~http_server();

    // Setup the addrinfo struct for this server.
    void initialize();

  private:

    // Main socket file descriptor.
    int m_sfd{-1};

    // Socket file descriptor which has accepted a connection.
    int m_accepted{-1};

    std::string m_port{"2490"};

    addrinfo* m_addr{nullptr};
};


#endif
