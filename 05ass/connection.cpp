#include <sstream>
#include <cctype>
#include <cassert>
#include <string.h>
#include <string>
#include <iostream>
#include "csapp.h"
#include "message.h"
#include "connection.h"

using std::string;
using std::cerr;
using std::cout;
using std::endl;

Connection::Connection()
  : m_fd(-1)
  , m_last_result(SUCCESS) {
}

Connection::Connection(int fd)
  : m_fd(fd)
  , m_last_result(SUCCESS) {
  // TODO: call rio_readinitb to initialize the rio_t object
  rio_readinitb(&m_fdbuf, m_fd);
}

void Connection::connect(const std::string &hostname, int port) {
  // TODO: call open_clientfd to connect to the server
  string port_s = std::to_string(port);
  const char* port_c = port_s.c_str();
  const char* hostname_c = hostname.c_str();
  int client_fd = open_clientfd(hostname_c, port_c); //? accepts hostname (server address) as a string and the desired port as a string
     //* ^ returns a file descriptor (the 'socket') - client socket
  if (client_fd < 0) {
    printf("Error: open_clientfd failed"); // can't connect to server
    exit(-1);
  }
  // TODO: call rio_readinitb to initialize the rio_t object
  rio_readinitb(&m_fdbuf, client_fd); //?? connect client with ?m_fdbuf?????
}

Connection::~Connection() {
  // TODO: close the socket if it is open
  close();
}

bool Connection::is_open() const {
  // TODO: return true if the connection is open
  return m_fd > 0;
}

void Connection::close() {
  // TODO: close the connection if it is open
  if (is_open()) {
    Close(m_fd);
  }
}

bool Connection::send(const Message &msg) {
  // TODO: send a message
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately

  int len = msg.get_msg_len(); //? idk if my helper will work
  //? ^ do we need to also send message length
  const char* buf = msg.data.c_str();
  if (rio_writen(m_fd, buf, len) < 0) { // send to m_fd (server's ip addr)
    m_last_result = EOF_OR_ERROR; // error sending data
    return false; 
  }
  return true;

}

bool Connection::receive(Message &msg) {
  // TODO: receive a message, storing its tag and data in msg
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately

  // read length and data ??
  char buf[Message::MAX_LEN]; // allocate buffer for message data
  ssize_t readlen = rio_readlineb(&m_fdbuf, buf, sizeof(buf)); // read message from m_fdbuf server, store in buf, read up to max_len
  rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen)
  if (readlen < 0) {
    cerr << "Error: rio_readlineb failed" << endl;
    return false; // error
  } else if (readlen == 0) { // Interrupted by sig handler return
    return false; // connection closed
  }

  //?? Extract the tag and data from the buffer???
  string tag, data;
  msg = Message(tag, data);


  int bytes_received
}
