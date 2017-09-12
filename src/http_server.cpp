#include "http_server.hpp"


http_server::
http_server(std::string _port, std::string _root_dir) : m_port(_port),
  m_root_dir(_root_dir) {
  initialize();

  m_sfd = socket(m_addr->ai_family, m_addr->ai_socktype, m_addr->ai_protocol);
  if(m_sfd == -1)
    std::cerr << "Something went wrong construction the socket!" << std::endl;

  if(bind(m_sfd, m_addr->ai_addr, m_addr->ai_addrlen) == -1)
    std::cerr << "Something went wrong when binding the socket!" << std::endl;

  if(listen(m_sfd, 10) == -1)
    std::cerr << "Something went wrong when listening!" << std::endl;

  sockaddr peer_addr;
  socklen_t addr_len = (socklen_t) sizeof(sockaddr);
  m_accepted = accept(m_sfd, (sockaddr*) &peer_addr, &addr_len);

  if(m_accepted == -1)
    std::cerr << "Something went wrong when accepting!" << std::endl;

  for(;;) {
    char* buffer = new char[1028];
    if(recv(m_accepted, buffer, 1028, 0) == -1)
      std::cerr << "Failed to receive data!" << std::endl;

    std::string msg = std::string(buffer);

    if(!msg.empty())
      std::cout << msg << std::endl;

    delete[] buffer;
  }
}


http_server::
~http_server() {
  close(m_sfd);
  close(m_accepted);
}


void
http_server::
initialize() {
  addrinfo hints;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  getaddrinfo(0, m_port.c_str(), &hints, &m_addr);

  std::cout << "HTTP server initialized on port " << m_port << "." << std::endl;
}
