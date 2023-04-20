#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "client_util.h"

using std::cerr;
using std::endl;

void read_cmd_loop(Connection& conn) {
  bool quitted;
  while(!quitted) {
    std::string input;
    std::getline(std::cin, input);
    
    Message msg;
    if (input.at(0) == '/') { // check that it's a command
      if (input.find("/join") == 0) { // create messages accordingly
        std::string room_num = input.substr(6);
        msg = Message(TAG_JOIN, room_num);
      } else if (input.find("/leave") == 0) {
        msg = Message(TAG_LEAVE, "ignored");
      } else if (input == "/quit") {
        msg = Message(TAG_QUIT, "bye");
        quitted = true;
      } else {
        cerr << "Invalid command" << endl;
        continue;
      }
    } else { // sendall
      msg = Message(TAG_SENDALL, input);
    }

    if(!conn.checkResponse(msg)) { // sends message
      continue; // deal with error and continue
    } 
  }
}
int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "Usage: ./sender [server_address] [port] [username]\n";
    return 1;
  }

  std::string server_hostname;
  int server_port;
  std::string username;

  server_hostname = argv[1];
  server_port = std::stoi(argv[2]);
  username = argv[3];

  // connect to server
  Connection conn;
  conn.connect(server_hostname, server_port);
  if (!conn.is_open()) {
    cerr << "Error: connection failed" << endl;
    return 1;
  }

  // send slogin message
  Message msg(TAG_SLOGIN, username);
  if (!conn.checkResponse(msg)) {
    conn.close();
    return 1;
  }

  // loop reading commands from user, sending messages to server as appropriate
  read_cmd_loop(conn);

  conn.close();
  return 0;
}

