#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"
#include "room.h"

Room::Room(const std::string &room_name)
  : room_name(room_name) {
  // initialize the mutex
  pthread_mutex_init(&lock, NULL);
}

Room::~Room() {
  // destroy the mutex
  pthread_mutex_destroy(&lock);
}

void Room::add_member(User *user) {
  // check if user is in the room before adding
  if (!members.count(user)) {
    Guard guard(lock);
    members.insert(user);
  }
}

void Room::remove_member(User *user) {
  // remove User from the room
  Guard guard(lock);
  members.erase(user);
  
}

void Room::broadcast_message(const std::string &sender_username, const std::string &message_text) {
  // TODO: send a message to every (receiver) User in the room
  
}
