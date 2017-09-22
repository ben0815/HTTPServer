#include "http_server.hpp"


http_server::
http_server(std::string _port, std::string _root) : m_port(_port),
  m_root(_root) {
  // Setup address information.
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

  // Close the files we opened.
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
    // Accept incoming connections.
    m_accepted = accept(m_sfd, (sockaddr*) &peer_addr, &addr_len);

    if(m_accepted == -1) {
      perror("Accept failed");
      throw std::runtime_error("Failed to accept connections to master socket "
          "with file descriptor " + std::to_string(m_sfd) + ".");
    }

    // Continuously receive and serve requests from the client.
    char* buffer = new char[8912];
    if(recv(m_accepted, buffer, 8912, 0) == -1) {
      perror("Receive failed");
      throw std::runtime_error("Failed to receive data from accepting socket "
          "with file descriptor " + std::to_string(m_accepted) + ".");
    }

    const std::string request(buffer);

    // Send the request to the parser to make sure it is a GET request.
    // Handle request appropriately.
    parse(request);

    delete[] buffer;
    close(m_accepted);
  }
}


void
http_server::
parse(const std::string& _request) const {
  // Parse off the request line and the method.
  const std::string req_line(_request, 0, _request.find("\n")),
                    method(req_line, 0, req_line.find(" "));

  // For this assignment only GET methods are allowed.
  if(method == "GET")
    handle_get_request(req_line);
  else
    error_response("405 Method Not Allowed");
}


void
http_server::
handle_get_request(const std::string& _req_line) const {
  std::string uri = "";

  // Get the requested uri.
  {
    const size_t start = _req_line.find(" ") + 1,
                 num = _req_line.rfind(" ") - start;

    uri = _req_line.substr(start, num);
  }

  // Keep URIs under 2000 characters. This makes them work on all browsers.
  if(uri.size() > 2000) {
    error_response("414 URI Too Long");
    return;
  }

  // Make sure '~' and '..' do not exist in the URI to limit access to files
  // outside the root directory.
  if(uri.find("~") != std::string::npos or uri.find("..") != std::string::npos) {
    error_response("403 Forbidden");
    return;
  }

  std::string path = "";

  // Append the uri to the root to get the path of the requested resource.
  if(m_root.at(m_root.size() - 1) == '/')
    path = m_root.substr(0, m_root.size() - 1) + uri;
  else
    path = m_root + uri;

  // If a directory is requested try to serve index.html in that directory.
  if(uri.at(uri.size() - 1) == '/')
    path += "index.html";

  std::string extension = "";

  // Get the file extension of the requested resource if applicable.
  {
    const size_t start = path.rfind("."),
                 num = path.size() - start;

    extension = path.substr(start, num);
  }

  // Check if we have a valid file extension. Also get the content type.
  const auto& valid = is_valid_ext(extension);

  if(!valid.first) {
    error_response("415 Unsupported Media Type");
    return;
  }

  // Get the type of content from the validity checker.
  const auto& content_type = valid.second;

  // If the resource exists and it is valid, serve it.
  serve(content_type, path);
}


void
http_server::
serve(const std::string& _content_type, const std::string& _path) const {
  // Get the type because text files need to be read differently than image files.
  const std::string type(_content_type, 0, _content_type.find("/"));

  auto mode = std::fstream::in;

  // If the resource is an image we need to open it in binary mode.
  if(type == "image")
    mode |= std::fstream::binary;

  // Try to open the resource.
  std::ifstream ifs(_path, mode);
  if(!ifs) {
    error_response("404 Not Found");
    ifs.close();
    return;
  }

  // Read the resource content into a string.
  const std::string content{std::istreambuf_iterator<char>(ifs),
      std::istreambuf_iterator<char>()};

  // Construct the HTTP response.
  const std::string response = "HTTP/1.1 200 OK\r\nServer: Ben's HTTP server.\r\n"
    "Content-Length: " + std::to_string(content.size()) + "\r\nContent-Type: " +
    _content_type + "\r\nConnection: Closed\r\n\r\n" + content;

  send(m_accepted, response.c_str(), response.size(), 0);
}


void
http_server::
error_response(const std::string& _error) const {
  // Send an error to the client.
  const std::string response = "HTTP/1.1 " + _error + "\r\n";
  send(m_accepted, response.c_str(), response.size(), 0);
}


std::pair<bool, std::string>
http_server::
is_valid_ext(const std::string& _extension) const {
  // Whitelist of file extensions. These are the file types that this
  // server can handle.
  static std::vector<std::string> white_list{".txt", ".html", ".jpg", "jpeg",
    ".png", ".ico", ".gif"};

  const auto& it = std::find(white_list.cbegin(), white_list.cend(), _extension);

  // False if we our extension is not in the white list.
  if(it == white_list.cend())
    return std::make_pair(false, "");

  std::string content_type = "";

  // If extension is valid determine its content type.
  if(_extension == ".txt")
    content_type = "text/plain; charset=utf-8";
  else if(_extension == ".html")
    content_type = "text/html; charset=utf-8";
  else if(_extension == ".jpg" or _extension == ".jpeg")
    content_type = "image/jpeg";
  else if(_extension == ".png")
    content_type = "image/png";
  else if(_extension == ".ico")
    content_type = "image/ico";
  else if(_extension == ".gif")
    content_type = "image/gif";
  else
    throw std::runtime_error("Extension invalid after it passed validity test.");

  return std::make_pair(true, content_type);
}
