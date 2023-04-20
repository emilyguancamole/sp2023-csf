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

  // TODO: connect to server
  Connection conn;
  conn.connect(server_hostname, server_port);
  if (!conn.is_open()) {
    cerr << "Error: connection failed" << endl;
    return 1;
  }

  // TODO: send slogin message
  Message msg(TAG_SLOGIN, username);
  if (!conn.checkResponse(msg)) {
    cerr << "Error: " << msg.data << endl; //?? how to format errors... payload?  or custom "failed to send slogin message"
    conn.close();
    return 1;
  }

  // TODO: loop reading commands from user, sending messages to
  //       server as appropriate
  bool quitted;

  while(!quitted) {
    std::string input;
    std::getline(std::cin, input);

    Message msg;

    if (input.substr(0, 6) == "/join ") {
      msg = Message(TAG_JOIN, input.substr(7));
    } else if (input == "/leave") {
      msg = Message(TAG_LEAVE, "left the room");
    } else if (input == "/quit") {
      msg = Message(TAG_QUIT, "bye!");
      quitted = true;
    } else { //?? All other commands should be rejected with an error message printed to stderr/cerr
      msg = Message(TAG_SENDALL, input);
    }

    if(!conn.checkResponse(msg)) {
      cerr << "Error: failed to send message" << endl;
      conn.close();
      return 1;
    }
  }

  conn.close();
  return 0;
}
