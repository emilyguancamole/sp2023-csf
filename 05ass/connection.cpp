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
  // call rio_readinitb to initialize the rio_t object
  rio_readinitb(&m_fdbuf, m_fd);
}
// ??what is m_fd - file descriptor of the socket (ip addr and port number of a connection). 
    // m_fd describes the connection between client and server
// ??what is rio - rio is a buffer that stores data read from the socket
    // what data is read from the socket? - data sent by client, such as login message, message to be sent.

void Connection::connect(const std::string &hostname, int port) {
  // call open_clientfd to connect to the server
  string port_s = std::to_string(port);
  const char* port_c = port_s.c_str();
  const char* hostname_c = hostname.c_str();
  m_fd = open_clientfd(hostname_c, port_c); // accepts hostname (server address) and desired port as string

  if (m_fd < 0) {
    cerr << "Error: open_clientfd failed"; // can't connect to server
    exit(-1);
  }

  // call rio_readinitb to initialize the rio_t object
  rio_readinitb(&m_fdbuf, m_fd);
}

Connection::~Connection() {
  // close the socket if it is open
  if (is_open()) {
    close();
  }
  
}

bool Connection::is_open() const {
  // return true if the connection is open
  return m_fd > 0;
}

void Connection::close() {
  // close the connection if it is open
  if (is_open()) {
    Close(m_fd);
  }
  m_fd = -1; // invalid fd
}

bool Connection::send(const Message &msg) {
  // send a message. Return true if successful, false if not. Set m_last_result appropriately

  // check the validity of the message data by checking length
  if (msg.data.length() > Message::MAX_LEN) {
    m_last_result = INVALID_MSG;
    return false;
  }
  // check the validity of the message tag
  if (msg.tag.length() == 0) {
    m_last_result = INVALID_MSG;
    return false;
  }

  // FORMAT the message into string, convert to cstring - to be displayed
  string msgstr = msg.tag + ":" + msg.data + "\n"; 
  
  const char* msg_cstr = msgstr.c_str();
  ssize_t msg_size = strlen(msg_cstr);
  ssize_t msg_size_sent = rio_writen(m_fd, msg_cstr, strlen(msg_cstr)); // call rio_writen to send

  // check if message was sent successfully - entire message sent
  if (msg_size == msg_size_sent) {
    m_last_result = SUCCESS;
  } else {
    m_last_result = EOF_OR_ERROR;
    return false;
  }
  return true;
}

bool Connection::receive(Message &msg) {
  // receive a message, storing its tag and data in msg
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately

  char buf[Message::MAX_LEN]; // allocate buffer for message data
  // read message from m_fdbuf server, store in buf, read up to max_len
  ssize_t readlen = rio_readlineb(&m_fdbuf, buf, sizeof(buf)); 
  if (readlen <= 0) {
    m_last_result = EOF_OR_ERROR;
    return false;
  } 

  // extract the tag and data from the buffer
  string message = buf;
  int colon = message.find(':'); // position of colon found to separate tag and data
  string tag = message.substr(0, colon);
  string data = message.substr(colon+1);
  msg = Message(tag, data); // store in msg

  m_last_result = SUCCESS;
  return true;
}

bool Connection::checkResponse (Message &msg) {
  // send message and check
  if (!send(msg)) { // msg is the message to send to server
    cerr << msg.data; // error message
    return false;
  }
  // the send was successful! now we receive message and check
  // msg is now the received message from server
  if (!receive(msg) || msg.tag == TAG_ERR) { // if didn't receive or tag error, then return false to print payload in caller
    cerr << msg.data;
    return false;
  } else if (msg.tag != TAG_OK) { // check if the tag is valid and print out custom error
    cerr << "Error: request did not run to completion" << endl;
    return false;
  } 

  return true;
}