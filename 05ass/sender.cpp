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
  try {
    Message msg = Message (TAG_SLOGIN, username);
    conn.checkResponse(msg);
  } catch (std::runtime_error &e) {
    cerr << "Error: failed to send slogin message" << endl;
    conn.close();
    return 1;
  }

  // TODO: loop reading commands from user, sending messages to server as appropriate
  bool quitted;

  while(!quitted) {
    std::string input;
    std::getline(std::cin, input);
    //std::string token = input.substr(0, input.find(" ")); // get the first word, which is the command

    Message msg;

    if (input.substr(0, 6) == "/join ") {
      msg = Message(TAG_JOIN, input.substr(6));
      //if (!conn.checkResponse) { // if pass conn as pointer, use ->
        // error msg.data
      //}
    } else if (input == "/leave") {
      msg = Message(TAG_LEAVE, "ignored");
      // same thing
    } else if (input == "/quit") {
      msg = Message(TAG_QUIT, "bye");
      quitted = true;
      // same checkresponse
      
      //conn.checkResponse(msg);
      //break;
    } else { // sendall
      // same thing: checkrsponse and if error do msg.data
      msg = Message(TAG_SENDALL, input);
    }
    /* else if (token.at(0)) { //?? All other commands should be rejected with an error message printed to stderr/cerr
      cerr << "Invalid command" << endl; */

    if (!conn.send(msg)) {
      cerr << "Error: failed to send message" << endl;
      conn.close();
      return 1;
    }

    try {
      conn.checkResponse(msg);
    } catch (std::runtime_error &e) {
      cerr << "Error: Runtime Error" << endl;
      continue;
    }
  }

  conn.close();
  return 0;
}

