#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"
#include "room.h"
#include <iostream>

// helper function to trim string of whitespace
std::string rtrim(const std::string &s) {
    const std::string WHITESPACE = " \n\r\t\f\v";
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

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
  if (members.count(user) == 0) {
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
  Guard guard(lock); // ta: guard entire broadcast function
  string message = rtrim(room_name) + ":" + rtrim(sender_username) + ":" + rtrim(message_text);
  std::set<User*>::iterator it;
  //std::cout << "num elements in members: " << members.size() << std::endl;
  for (it = members.begin(); it != members.end(); it++) {
    User* curr_user = *it;
    Message* b_message = new Message(TAG_DELIVERY, message);
    (curr_user->mqueue).enqueue(b_message);
  }

  /*
  for (User* user : members) {
    Message* b_message = new Message(TAG_DELIVERY, message);
    std::cout << message << std::endl;
    user->mqueue.enqueue(b_message);
  }
  */
}

int Room::get_room_size() { return members.size(); }