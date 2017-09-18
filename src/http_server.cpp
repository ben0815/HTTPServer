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

  for(;;) {
    m_accepted = accept(m_sfd, (sockaddr*) &peer_addr, &addr_len);

    if(m_accepted == -1) {
      perror("Accept failed");
      throw std::runtime_error("Failed to accept connections to master socket "
          "with file descriptor " + std::to_string(m_sfd) + ".");
    }

    // Continuously receive and serve requests from the client.
    char* buffer = new char[2048];
    if(recv(m_accepted, buffer, 2048, 0) == -1) {
      perror("Receive failed");
      throw std::runtime_error("Failed to receive data from accepting socket "
          "with file descriptor " + std::to_string(m_accepted) + ".");
     }

    const auto request = std::string(buffer);
    if(request.size() > 1024)
      throw std::runtime_error("Cannot service request's longer than 1 KB.");

    if(!request.empty())
      std::cout << request << std::endl;

    // Send the request to the parser to make sure it is a GET request.
    // Handle request appropriately.
    parse(request);

  //const char message[] = "HTTP/1.1 200 Okay\r\nContent-Type: text/html; charset=ISO-8859-4 \r\n\r\n<h1> Hi! <h1>";

    delete[] buffer;
    close(m_accepted);
  }
}


void
http_server::
parse(const std::string& _request) const {
  const auto req_line = _request.substr(0, _request.find("\n")),
             method = req_line.substr(0, req_line.find(" "));

  if(method == "ET")
    handle_get_request(req_line);
  else
    error_response("405 Method Not Allowed");
}


void
http_server::
handle_get_request(const std::string& _req_line) const {
}


void
http_server::
error_response(const std::string& _error) const {
    const std::string response = "HTTP/1.1 " + _error + " \r\n";
    send(m_accepted, response.c_str(), response.size(), 0);
}
