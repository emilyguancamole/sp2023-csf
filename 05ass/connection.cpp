#include <sstream>
#include <cctype>
#include <cassert>
#include <string>
#include "csapp.h"
#include "message.h"
#include "connection.h"

using std::string;

Connection::Connection()
  : m_fd(-1)
  , m_last_result(SUCCESS) {
}

Connection::Connection(int fd)
  : m_fd(fd)
  , m_last_result(SUCCESS) {
  // TODO: call rio_readinitb to initialize the rio_t object
  rio_t rio; //? rio_t: data type wrapping a file descriptor and allowing buffered input
  rio_readinitb(&rio, fd);
}

void Connection::connect(const std::string &hostname, int port) {
  // TODO: call open_clientfd to connect to the server
  string port_s = std::to_string(port);
  const char* port_c = port_s.c_str();
  const char* hostname_c = hostname.c_str();
  int client_fd = open_clientfd(hostname_c, port_c); //? accepts hostname (server address) as a string and the desired port as a string
     //* ^ returns a file descriptor (the 'socket')
  // TODO: call rio_readinitb to initialize the rio_t object
  rio_t rio;
  rio_readinitb(&rio, client_fd);
}

Connection::~Connection() {
  // TODO: close the socket if it is open
}

bool Connection::is_open() const {
  // TODO: return true if the connection is open
}

void Connection::close() {
  // TODO: close the connection if it is open
}

bool Connection::send(const Message &msg) {
  // TODO: send a message
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
}

bool Connection::receive(Message &msg) {
  // TODO: receive a message, storing its tag and data in msg
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
}
