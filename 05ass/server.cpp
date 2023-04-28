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
#include "message_queue.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "server.h"

using std::cerr;

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////

// TODO: add any additional data types that might be helpful
//       for implementing the Server member functions

struct Client {
  Server* server;
  Connection* conn;

  // destructor - delete conn
};

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {
// helper function when client logged in as receiver
void chat_with_receiver(Client& client, string& username) {
  // register receiver thread with username //?? where does username come up
  Message join_msg;
  // determine tag of message to check if it's join (what we're expecting). if not, output error: ""Error: message not received";
  if (!client.conn->receive(join_msg) || join_msg.tag != TAG_JOIN) {
    join_msg.tag = TAG_ERR;
    join_msg.data = "Error: not able to join room";
    client.conn->send(join_msg);
    return;
  } 

  User *user = new User(username); // make dynam-alloc user with username
  // register receiver to room. after getting join msg, get room number
  string room_name = join_msg.data;
  std::cout << "ROOM Name" << room_name << std::endl;
  Room* room = client.server->find_or_create_room(room_name);
  std::cout << "ROOM NAME ROOM " << room->get_room_name() << std::endl;
  room->add_member(user); // only add receivers as member to roomn //!

    // if good, send ok
  Message ok_msg = Message(TAG_OK, "welcome");
  if (!client.conn->send(ok_msg)) {
    return;
  }

  // start receiving msgs in a loop, within that room
  bool go = true;
  while (go) {
    Message* mq = (user->mqueue).dequeue(); // dequeue next message to receive //??can we use same msg here
    //std::cout << "msg.data in SEND !! " << mq->data << std::endl;
    if(mq == nullptr) {
      go = false;
      continue;
    }
    if(!client.conn->send(*mq)) { // sends the sendall message
      go = false; // break out of while loop if failed to send
      //continue;
    }
    delete mq;
  }

  // delete remaining messages on the queue
  // user->mqueue.clear_queue(); // if this breaks things, delete it
  
  room->remove_member(user); // frees the user
  delete user;
}

void chat_with_sender(Client& client, string& username) {
  Room *sender_room = NULL;

  bool quitted = false;
  while(!quitted) { //?? need to check if we received message from the user??    
    Message msg;
    if (client.conn->receive(msg)) { //? th
      if (msg.tag == TAG_JOIN) {
        sender_room = client.server->find_or_create_room(msg.data);
        std::cout << "SENDER ROOM NAME " << sender_room->get_room_name() << std::endl;
        if (sender_room == NULL) {
          msg.tag = TAG_ERR;
          msg.data = "Error: failed to join room";
          client.conn->send(msg);
        } 
        client.conn->send(Message(TAG_OK, "welcome"));
      } else if (msg.tag == TAG_SENDALL) {
        sender_room->broadcast_message(username, msg.data);
        msg.tag = TAG_OK;
        msg.data = "ok";
        client.conn->send(msg);
      } else if (msg.tag == TAG_LEAVE) {
        sender_room = NULL;
        msg.tag = TAG_OK;
        msg.data = "leave";
        client.conn->send(msg);
      } else if (msg.data == TAG_QUIT) {
        msg.tag = TAG_OK;
        msg.data = "bye";
        client.conn->send(msg);
        quitted = true;
      } else {
        if (client.conn->get_last_result() == Connection::EOF_OR_ERROR) {
          msg.tag = TAG_ERR;
          msg.data = "Error: invalid message tag";
          client.conn->send(msg);
        }
      }
    }
  }
}

void *worker(void *arg) { // entry point for new thread being created
  // thread is already created
  pthread_detach(pthread_self()); // * makes it start working on its own. when it finishes, it will return, so function wont wait for it to finish

  // TODO: use a static cast to convert arg from a void* to
  //       whatever pointer type describes the object(s) needed
  //       to communicate with a client (sender or receiver)
  Client* client = static_cast<Client*>(arg);

  // TODO: read login message (should be tagged either with TAG_SLOGIN or TAG_RLOGIN), 
  //       send response
  Message login_msg;
  if (!client->conn->receive(login_msg)) {
    if (client->conn->get_last_result() == Connection::EOF_OR_ERROR) {
      login_msg.tag = TAG_ERR;
      login_msg.data = "Error: failed to receive login message";
      client->conn->send(login_msg);
      return nullptr;
    }
  }; // call receive function, store message in login_msg
  if (login_msg.tag != TAG_SLOGIN && login_msg.tag != TAG_RLOGIN) {
    // send a error msg back with tag error
    login_msg.tag = TAG_ERR;
    login_msg.data = "Error: login message has wrong tag";
    client->conn->send(login_msg);
    return nullptr;
  }
  // send response
  Message ok_msg = Message(TAG_OK, "logged in");
  if (!client->conn->send(ok_msg)) {
    login_msg.tag = TAG_ERR;
    login_msg.data = "Error: failed to send message";
    client->conn->send(login_msg);
    return nullptr;
  }

  // TODO: depending on whether the client logged in as a sender or
  //       receiver, communicate with the client (implementing
  //       separate helper functions for each of these possibilities is a good idea)
  // Message join_msg;
  if (login_msg.tag == TAG_RLOGIN) { // receiver
    chat_with_receiver(*client, login_msg.data);
  } else if (login_msg.tag == TAG_SLOGIN) { // receiver
    chat_with_sender(*client, login_msg.data);
  }

  // free everything in client
  delete client->conn;
  //delete client->server;
  delete client;

  return nullptr;
}

}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
  : m_port(port)
  , m_ssock(-1) {
  // TODO: initialize mutex
  pthread_mutex_init(&m_lock, NULL);
}

Server::~Server() {
  // TODO: destroy mutex
  // mutex_destroy destroys underlying lock
  pthread_mutex_destroy(&m_lock);
}

bool Server::listen() {
  // TODO: use open_listenfd to create the server socket, return true if successful, false if not
  string port_s = std::to_string(m_port);
  const char* port_c = port_s.c_str();
  m_ssock = open_listenfd(port_c);
  if (m_ssock < 0) {
    //cerr << "Error: open_listenfd failed";
    return false;
  }
  return true;
}

void Server::handle_client_requests() {
  // TODO: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client

  // need to call pthread_create whenever the client sends a request
  while(true) {
    int clientfd = accept(m_ssock, NULL, NULL); // fd for the connection to client
    if (clientfd < 0) {
      cerr << "Error accepting client connection";
      continue;
    }

    Client* new_client = new Client();
    new_client->conn = new Connection(clientfd);
    new_client->server = this;
    pthread_t thr_id;
    if (pthread_create(&thr_id, NULL, worker, new_client) != 0) { // worker is the entry pt for the new thread to execute
      continue;
    }

    //* each client (receiver or sender) has its own thread -> spawn a new thread with each client -> allows them to run in parallel
    //      many fds for senders/receivers. instead of round-robingin and potentially getting stuck on a sender or receiver
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // TODO: return a pointer to the unique Room object representing
  //       the named chat room, creating a new one if necessary
  Guard guard(m_lock); // m_rooms is not thread-safe
  Room* room;
  if (m_rooms.find(room_name) == m_rooms.end()) {
    room = new Room(room_name); // create a new room
    m_rooms.insert(std::pair<string, Room*>(room_name, room));
  } else {
    room = m_rooms.at(room_name);
  }
  return room;
}
