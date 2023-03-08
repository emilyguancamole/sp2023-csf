#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include "functions.h"

using std::string;
using std::cout;
using std::cin;

int main(int argc, char* argv[]) {
    
    int sets = std::stoi(argv[1]);
    // check to ensure sets is a positive power of 2
    if (!isPowOfTwo(sets)) {
      return 1;
    }

    int blocks = std::stoi(argv[2]); 
    int bytes = std::stoi(argv[3]); // block size 

    string write_func = argv[4];
    string through_back = argv[5];
    string evictions = argv[6];

    string line;
    char command;
    uint32_t address;
    uint32_t offset_size = log2(bytes); // ex. passing 16 would give size=4
    uint32_t index_size = log2(blocks);
    uint32_t tag_size = 32 - offset_size - index_size; 
    uint32_t index;
    uint32_t tag;

    
    while (std::getline(cin, line)) { // read line by line
        std::istringstream ss(line); // store line into string buffer
        ss >> command;
        ss >> std::hex >> address; // parse as hexadecimal
        ss.clear(); // last element isn't used

        index = address; // make a copy of address
        index = index << tag_size >> (tag_size + offset_size); // shift to only get index
        tag = address;
        tag = tag >> (index_size + offset_size); // only get tag

        
        cout << "address: " << address << "\n";
        cout << "tag: " << tag << "\n";
        cout << "index: " << index << "\n";

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