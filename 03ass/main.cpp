#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "functions.h"

using std::string;

int main(int argc, char* argv[]) {

    int sets = std::stoi(argv[1]);
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
    string command;
    string address;
    while (std::getline(in, line)) { // read line by line
        std::istringstream ss(line); // store line into string buffer
        ss >> command;
        ss >> address;


    }
    

    return 0;
}