#include <cstddef>
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

struct Client { // represents a client for a chatroom
  Server* server;
  Connection* conn;
};

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////

namespace {
// helper function when client logged in as receiver
void chat_with_receiver(Client& client, string& username) {
  // register receiver thread with username
  Message join_msg;
  // determine tag of message to check if it's join (what we're expecting);
  if (!client.conn->receive(join_msg) || join_msg.tag != TAG_JOIN) {
    client.conn->send(Message(TAG_ERR, "not able to join room"));
    return;
  } 

  User *user = new User(username); // make dynam-alloc user with username
  // register receiver to room. after getting join msg, get room number
  string room_name = join_msg.data;
  Room* room = client.server->find_or_create_room(room_name);
  room->add_member(user); // only add receivers as member to room
  Message ok_msg = Message(TAG_OK, "welcome"); // if good, send ok
  if (!client.conn->send(ok_msg)) {
    return;
  }

  // start receiving msgs in a loop, within that room
  bool go = true;
  while (go) {
    Message* mq = (user->mqueue).dequeue(); // dequeue next message to receive
    if(mq != nullptr) {
      if(!client.conn->send(*mq)) { // sends the sendall message
        go = false; // break out of while loop if failed to send
      }
    }
    delete mq;
  }

  // delete remaining messages on the queue
  room->remove_member(user); // frees the user
  delete user;
}

void chat_with_sender(Client& client, string& username) {
  Room *sender_room = NULL;

  bool quitted = false;
  while(!quitted) {   
    Message msg;
    if (client.conn->receive(msg)) {
      if (msg.tag == TAG_JOIN) {
        sender_room = client.server->find_or_create_room(msg.data);
        if (sender_room == NULL) {
          client.conn->send(Message(TAG_ERR, "failed to join room"));
        } 
        client.conn->send(Message(TAG_OK, "welcome"));
      } else if (msg.tag == TAG_SENDALL) {
        if (sender_room != NULL) { // make sure in a room before sending
          sender_room->broadcast_message(username, msg.data);
          client.conn->send(Message(TAG_OK, "message sent"));
        } else {
          client.conn->send(Message(TAG_ERR, "not in a room"));
        }
      } else if (msg.tag == TAG_LEAVE) {
        if (sender_room != NULL && sender_room->get_room_name() == msg.data) { // only leave if already in the same room
          sender_room = NULL;
          client.conn->send(Message(TAG_OK, "left room"));
        } else {
          client.conn->send(Message(TAG_ERR, "not in the room"));
        }
      } else if (msg.tag == TAG_QUIT) {
        quitted = true;
        client.conn->send(Message(TAG_OK, "bye"));
      } else {
        if (client.conn->get_last_result() == Connection::INVALID_MSG) { 
          client.conn->send(Message(TAG_ERR, "invalid message tag"));
        }
      }
    }
  }
}

void *worker(void *arg) { // entry point for new thread being created
  // thread is already created
  pthread_detach(pthread_self()); // * makes it start working on its own

  // use a static cast to convert arg from a void* to Client*
  Client* client = static_cast<Client*>(arg);

  // read login message (should be tagged either with TAG_SLOGIN or TAG_RLOGIN), send response
  Message login_msg;
  if (!client->conn->receive(login_msg)) {
    if (client->conn->get_last_result() == Connection::EOF_OR_ERROR) {
      client->conn->send(Message(TAG_ERR, "failed to receive login message"));
      return nullptr;
    }
  } // call receive function, store message in login_msg
  if (login_msg.tag != TAG_SLOGIN && login_msg.tag != TAG_RLOGIN) {
    // send a error msg back with tag error
    client->conn->send(Message(TAG_ERR, "login message has wrong tag"));
    return nullptr;
  }
  // send response
  Message ok_msg = Message(TAG_OK, "logged in");
  if (!client->conn->send(ok_msg)) {
    client->conn->send(Message(TAG_ERR, "failed to send message"));
    return nullptr;
  }

  // depending on whether the client logged in as a sender or receiver, communicate with the client
  if (login_msg.tag == TAG_RLOGIN) { // receiver
    chat_with_receiver(*client, login_msg.data);
  } else if (login_msg.tag == TAG_SLOGIN) { // sender
    chat_with_sender(*client, login_msg.data);
  }

  // free everything in client
  delete client->conn;
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
  // initialize mutex
  pthread_mutex_init(&m_lock, NULL);
}

Server::~Server() {
  // destroy mutex
  pthread_mutex_destroy(&m_lock);
}

bool Server::listen() {
  // use open_listenfd to create the server socket, return true if successful, false if not
  string port_s = std::to_string(m_port);
  const char* port_c = port_s.c_str();
  m_ssock = open_listenfd(port_c);
  if (m_ssock < 0) {
    return false;
  }
  return true;
}

void Server::handle_client_requests() {
  // infinite loop calling accept or Accept, starting a new pthread for each connected client
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
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  // Return a pointer to the unique Room object representing the named chat room, creating a new one if necessary
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
