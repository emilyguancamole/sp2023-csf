#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "functions.h"

using std::string;

int main(int argc, char* argv[]) {
    
    int sets = std::stoi(argv[1]);
    // check to ensure sets is a positive power of 2
    if (!isPowOfTwo(sets)) {
      // error, number of sets is not a pow of 2
    }
    //initialize_sets()

    int blocks = std::stoi(argv[2]);
    int bytes = std::stoi(argv[3]);
    string write_func = argv[4];
    string through_back = argv[5];
    string evictions = argv[6];

    string filename;
    std::cin >> filename;
    
    std::ifstream in(filename); // open the file, store to 'in'
    if (!in.is_open()) {
    std::cerr << "Error: could not open input file " << filename << "\n";
    return 1;
  }
    
    string line;
    char command;
    string address;
    while (std::getline(in, line)) { // read line by line
        std::istringstream ss(line); // store line into string buffer
        ss >> command;
        ss >> address;
        ss.clear(); // last element isn't used
        // done with reading line

        // some type of switch stmt to process command (s or l)
        if (command == 's') {

        } 
        else if (command == 'l') {

        }
    }
    

    return 0;
}

bool isPowOfTwo(int n) {
    if (n <= 0) {
        return false;
    }
    return (n & (n - 1)) == 0; // will be even
}