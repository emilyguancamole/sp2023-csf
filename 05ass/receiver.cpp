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
bool chatloop(Connection &conn, Message &msg, string room) {
  while (conn.receive(msg)) { // while can still receive

    if (msg.tag != TAG_DELIVERY) {
      cerr << "Failed to receive delivery message" << endl;
      continue; // continue to next message
    }

    // make sure message payload is in correct format (room:sender:message_text) and parse message
    vector<string> in_msg = msg.format_data();
    if (in_msg.size() != 3) {
      cerr << "Error: invalid message format" << endl;
      return false;
    }
    cout << in_msg[1] << ": " << in_msg[2]; // print sender and message

    // check to make sure if message's room matches
    if (in_msg[0] != room) {
      cerr << "Error: room does not match receiver" << endl;
      return false;
    }
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

  // connect to server
  conn.connect(server_hostname, server_port);
  if (!conn.is_open()) {
    cerr << "Error: connection failed" << endl;
    return 1;
  }

  // send rlogin and join messages (expect a response from the server for each one)
  Message rlog_message = Message(TAG_RLOGIN, username);
  // if send or receive is bad, return error
  if (!conn.checkResponse(rlog_message)) {
    conn.close();
    return 1;
  }

  // send join message
  Message join_message = Message(TAG_JOIN, room_name);
  if (!conn.checkResponse(join_message)) {
    conn.close();
    return 1;
  }
  
  // loop waiting for messages from server (which should be tagged with TAG_DELIVERY)
  Message msg;
  chatloop(conn, msg, room_name); // helper for the loop

  conn.close(); // close everything at the end
  return 0;
}

