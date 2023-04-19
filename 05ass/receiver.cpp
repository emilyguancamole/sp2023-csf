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
  // TODO: send rlogin and join messages (expect a response from the server for each one)
  Message rlog_message = Message(TAG_RLOGIN, username);
  if (!conn.send(rlog_message)) {
    cerr << "Error: invalid send rlogin:" << username << endl;
    exit(1); //?? what does exit cleanly mean
  }

  // TODO: loop waiting for messages from server
  //       (which should be tagged with TAG_DELIVERY)
  while (true) {
    Message msg;
    if (!conn.receive(msg) || msg.tag != TAG_DELIVERY) {
      cerr << "Failed to receive delivery message" << endl;
      return 1;
    }
    
    cout << username << ": " << msg.get_msg_text(msg.data, ":") << endl;
  }

  return 0;
}

// int chat() {

  
// }