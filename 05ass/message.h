#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <string>
using std::string;

struct Message {
  // An encoded message may have at most this many characters,
  // including the trailing newline ('\n'). Note that this does
  // *not* include a NUL terminator (if one is needed to
  // temporarily store the encoded message.)
  static const unsigned MAX_LEN = 255;

  std::string tag;
  std::string data;

  Message() { }

  Message(const std::string &tag, const std::string &data)
    : tag(tag), data(data) { }

  // TODO: you could add helper functions
  //? function definitions in here?
  int get_msg_len() const { 
    return tag.length() + data.length() + 2; 
  } //?? return length of message... or consider both +2 or +3 for \n or \r\n?

  // get username from payload of format room:sender:message_text	(for delivery tag)
  std::string get_msg_text(string msg, string delim) {
    size_t pos = 0;
    string output;
    pos = msg.find(delim); // find the delim
    msg.erase(0, pos+delim.length()); // erase beginning of string up to delim
    pos = msg.find(delim); // find the delim again
    msg.erase(0, pos+delim.length()); // erase beginning of string up to delim
    pos = msg.find(delim); // find delim again
    output = msg.substr(0, pos);
    return output; // return the third part, which is message text

  
    // while ((pos=msg.find(delim) != string::npos)) { // while we can still find the delim
    //   token = msg.substr(0, pos);
    //   output.append(token);
    //   msg.erase(0, pos+delim.length()); // erase beginning of string
    // }
  }

};

// standard message tags (note that you don't need to worry about
// "senduser" or "empty" messages)
#define TAG_ERR       "err"       // protocol error
#define TAG_OK        "ok"        // success response
#define TAG_SLOGIN    "slogin"    // register as specific user for sending
#define TAG_RLOGIN    "rlogin"    // register as specific user for receiving
#define TAG_JOIN      "join"      // join a chat room
#define TAG_LEAVE     "leave"     // leave a chat room
#define TAG_SENDALL   "sendall"   // send message to all users in chat room
#define TAG_SENDUSER  "senduser"  // send message to specific user in chat room
#define TAG_QUIT      "quit"      // quit
#define TAG_DELIVERY  "delivery"  // message delivered by server to receiving client
#define TAG_EMPTY     "empty"     // sent by server to receiving client to indicate no msgs available

#endif // MESSAGE_H
