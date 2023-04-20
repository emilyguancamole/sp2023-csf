#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

using std::cerr;
using std::endl;
using std::cout;

// helper to loop waiting for message from server
bool chatloop(Connection &conn, Message &msg) {
  while (conn.receive(msg)) { // while can still receive

    if (msg.tag != TAG_DELIVERY) {
      cerr << "Failed to receive delivery message" << endl;
      // return 1;
      continue; //? do we do this to 'wait for next message' ? or return?
    }


    string msg_str = msg.data; // payload of message, format room:sender:message_text
    // split payload into room, sender, and text
    int colon = msg_str.find(':'); // position of colon
    string room = msg_str.substr(0, colon);
    msg_str.erase(0, colon); // erase room substring
    string data = msg_str.substr(colon+1);
    colon = msg_str.find(':'); // position of 2nd colon
    string sender = msg_str.substr(0, colon);
    string message_text = msg_str.substr(colon+1); // rest of the string is the actual message text

    // output sender and text
    cout << sender << ": " << message_text << endl;

    // todo: check to make sure if message's room matches
    
  }
  return true;
}

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  Connection conn;

  // TODO: connect to server
  conn.connect(server_hostname, server_port);
  if (!conn.is_open()) {
    cerr << "Error: connection failed" << endl;
    return 1;
  }

  // TODO: send rlogin and join messages (expect a response from the server for each one)
  Message rlog_message = Message(TAG_RLOGIN, username);
  // if send or receive is bad, return error
  if (!conn.checkResponse(rlog_message)) {
    cerr << "Error: invalid send for rlogin:" << username << endl;
    conn.close();
    return 1;
  }

  // send join message
  Message join_message = Message(TAG_JOIN, room_name);
  if (!conn.checkResponse(join_message)) {
    cerr << "Error: invalid send for join:" << room_name << endl;
    conn.close();
    return 1;
  }

  while (true) {
    Message msg;

    vector<string> in_msg = msg.format_data();
    if (in_msg.size() != 3) {
      cerr << "Error: invalid message format" << endl;
      return 1;
    }

    cout << in_msg[1] << ": " << in_msg[2] << endl;
  }
  
  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)
  Message msg;
  chatloop(conn, msg); // helper for the loop

  conn.close(); // close everything at the end
  return 0;
}

