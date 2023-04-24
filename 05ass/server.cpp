#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <vector>
#include <cctype>
#include <cassert>
#include "message.h"
#include "connection.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "server.h"

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////

// TODO: add any additional data types that might be helpful
//       for implementing the Server member functions

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {


void chat_with_receiver(Message& msg); // declaration
void chat_with_sender(Message& msg);

void *worker(void *arg) {
  pthread_detach(pthread_self());

  // TODO: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  Connection *conn = static_cast<Connection*>(arg);

  // TODO: read login message (should be tagged either with TAG_SLOGIN or TAG_RLOGIN), 
  //       send response
  Message login_msg;
  conn->receive(login_msg); // call receive function, store message in login_msg
  if (login_msg.tag != TAG_SLOGIN || login_msg.tag != TAG_RLOGIN) {
    login_msg.tag = TAG_ERR;
    login_msg.data = "Error: login message has wrong tag";
  }
  // send response
  if (!conn->send(login_msg)) {
    // ? 
  }

  // TODO: depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities is a good idea)

  if (login_msg.tag == TAG_RLOGIN) { // receiver
    chat_with_receiver(login_msg);
  } else if (login_msg.tag == TAG_SLOGIN) { // receiver
    chat_with_sender(login_msg);
  }

  return nullptr;
}

// helper function when client logged in as sender
void chat_with_receiver(Message& msg) {
  // register receiver thread with username //?? where does username come up
  pthread_t thid;
  void* conn; //???? new connection maybe idk?
  Pthread_create(&thid, NULL, worker, (void*)conn); // should do error checking itself?
  void *ret;
  Pthread_join(thid, &ret); // ret is value returned by exited thread (worker)

  // register receiver to room
}

// void chat_with_sender(Message& msg) {
//   // register receiver thread with username
//   pthread_t thid;
//   void* conn; //???? new connection maybe idk?
//   Pthread_create(&thid, NULL, worker, (void*)conn); // should do error checking itself?
//   void *ret;
//   Pthread_join(thid, &ret); // ret is value returned by exited thread (worker)
//   //?? where does username come up
// }

}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
  : m_port(port)
  , m_ssock(-1) {
  // TODO: initialize mutex
}

Server::~Server() {
  // TODO: destroy mutex
}

bool Server::listen() {
  // TODO: use open_listenfd to create the server socket, return true
  //       if successful, false if not
}

void Server::handle_client_requests() {
  // TODO: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
}
