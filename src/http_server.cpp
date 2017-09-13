#include "http_server.hpp"


http_server::
http_server(std::string _port, std::string _root_dir) : m_port(_port),
  m_root_dir(_root_dir) {
  addrinfo hints;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  getaddrinfo(0, m_port.c_str(), &hints, &m_addr);

  std::cout << "HTTP server initialized on port " << m_port << "." << std::endl;
}


http_server::
~http_server() {
  m_addr = nullptr;
  close(m_sfd);
  close(m_accepted);
}


void
http_server::
run() {
  // Construct a master socket.
  m_sfd = socket(m_addr->ai_family, m_addr->ai_socktype, m_addr->ai_protocol);
  if(m_sfd == -1) {
    perror("Socket failed");
    throw std::runtime_error("Socket failed.");
  }

  // Bind to port.
  if(bind(m_sfd, m_addr->ai_addr, m_addr->ai_addrlen) == -1) {
    perror("Bind failed");
    throw std::runtime_error("Bind failed on port " + m_port + ".");
  }

  // Mark master socket as passive socket.
  if(listen(m_sfd, 10) == -1) {
    perror("Listen failed");
    throw std::runtime_error("Failed to listen on master socket with file "
        "descriptor " + std::to_string(m_sfd) + ".");
  }

  // Get ready to accept connections.
  sockaddr peer_addr;
  socklen_t addr_len = (socklen_t) sizeof(sockaddr);
  m_accepted = accept(m_sfd, (sockaddr*) &peer_addr, &addr_len);

  if(m_accepted == -1) {
    perror("Accept failed");
    throw std::runtime_error("Failed to accept connections to master socket "
        "with file descriptor " + std::to_string(m_sfd) + ".");
  }

  // Continuously receive and serve requests from the client.
  for(;;) {
    char* buffer = new char[1028];
    close(m_accepted);
    if(recv(m_accepted, buffer, 1028, 0) == -1) {
      perror("Accept failed");
      throw std::runtime_error("Failed to receive data from accepting socket "
          "with file descriptor " + std::to_string(m_accepted) + ".");
     }

    std::string msg = std::string(buffer);

    if(!msg.empty())
      std::cout << msg << std::endl;

    delete[] buffer;
  }
}
