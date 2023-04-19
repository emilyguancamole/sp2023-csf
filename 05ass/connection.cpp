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
    // port > 1024
  if (client_fd < 0) {
    printf("Error: open_clientfd failed"); // can't connect to server
    exit(-1);
  }
  // TODO: call rio_readinitb to initialize the rio_t object
  rio_readinitb(&m_fdbuf, client_fd); //?? connect client with ?m_fdbuf?????
}

Connection::~Connection() {
  // TODO: close the socket if it is open
  if (is_open()) {
    close();
  }
  
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

  // FORMAT the message into string, convert to cstring
  string msgstr = msg.tag + ":" + msg.data;
  const char* msg_cstr = msgstr.c_str();
  ssize_t client_fd = rio_writen(m_fd, msg_cstr, strlen(msg_cstr)); // call rio_writen to send

  //?? CHECK ERRORS if open_clientfd returns < 0, print invalid message(??) and return
  if (client_fd < 0) {
    cerr << "Error: open_clientfd failed" << endl;
    m_last_result = EOF_OR_ERROR;
    return false;
  } else if (client_fd == 0) {
    m_last_result = EOF_OR_ERROR; //?? end of file, or different error? check the errors
    return false;
  } else {
    m_last_result = SUCCESS;
  }
  
  return true;
}

bool Connection::receive(Message &msg) {
  // TODO: receive a message, storing its tag and data in msg
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately

  char buf[Message::MAX_LEN]; // allocate buffer for message data
  // read message from m_fdbuf server, store in buf, read up to max_len
  ssize_t readlen = rio_readlineb(&m_fdbuf, buf, sizeof(buf)); 
  if (readlen < 0) {
    cerr << "Error: rio_readlineb failed" << endl;
    m_last_result = EOF_OR_ERROR; //?? when is it invalid message??
    return false; // error
  } else if (readlen == 0) { // Interrupted by sig handler return
    m_last_result = EOF_OR_ERROR;
    return false; // connection closed
  } else {
    m_last_result = SUCCESS;
  }

  // extract the tag and data from the buffer
  string message = buf;
  int colon = message.find(':'); // position of colon
  string tag = message.substr(0, colon);
  string data = message.substr(colon+1);
  msg = Message(tag, data);

  return true;
}

bool Connection::checkResponse (Message &msg) { 
  // send message and check
  if (!send(msg)) {
    return false;
  }
  // receive message and check
  if (!receive(msg) || msg.tag == TAG_ERR) {
    return false;
  }

  return true;
}