#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string.h>
#include "functions.h"

using std::string;
using std::cin;
using std::cerr;
using std::endl;
using std::exception;
using std::stoi;

bool isPowOfTwo(int n) {
    if (n <= 0) {
        return false;
    }
    return (n & (n - 1)) == 0; // will be even
}

bool isValidNum(string in) {
    try {
        stoi(in);
    } catch (exception &ex) {
        return false;
    }
    return true;
}

bool check_num_sets(Args *vals, string numSets) {
    if (!isValidNum(numSets)) { //check if the argument is valid
        cerr << "Given Number of Sets is Invalid!" << endl;
        return false;
    } else {
        vals->num_sets = stoi(numSets);
    }
    if (!isPowOfTwo(vals->num_sets)) { // check to ensure sets is a positive power of 2
        cerr << "Given Number of Sets is Not a Power of 2!" << endl;
        return false;
    }
    return true;
}

bool check_num_blocks(Args *vals, string numBlocks) {
    if (!isValidNum(numBlocks)) { //check if the argument is valid
        cerr << "Given Number of Blocks is Invalid!" << endl;
        return false;
    } else {
        vals->num_blocks_in_set = stoi(numBlocks);
    }
    if (!isPowOfTwo(vals->num_blocks_in_set)) { // check to ensure num of blocks is a positive power of 2
        cerr << "Given Number of Blocks is Not a Power of 2!" << endl;
        return false;
    }
    return true;
}

bool check_num_bytes(Args *vals, string numBytes) {
    if (!isValidNum(numBytes)) { //check if the argument is valid
        cerr << "Given Number of Bytes is Invalid!" << endl;
        return false;
    } else {
        vals->bytes = stoi(numBytes);
    }
    if (vals->bytes < 4 || !isPowOfTwo(vals->bytes)) { // check to ensure sets is a positive power of 2
        cerr << "Given Number of Blocks Must be a Power of 2 and Greater than 4!" << endl;
        return false;
    }
    return true;
}

bool check_write_func(Args *vals, string write_func) {
    if (write_func.compare("write-allocate") == 0){
        vals->write_alloc = true;
    } else if (write_func.compare("no-write-allocate") == 0) {
        vals->write_thru = false;
    } else {
        cerr << "Write Allocate should be write-through or no-write-allocate" << endl;
        return false;
    }
    return true;
}

bool check_through_back(Args *vals, string through_back) {
    if (through_back.compare("write-through") == 0){
        vals->write_thru = true;
    } else if (through_back.compare("write-back") == 0) {
        vals->write_thru = false;
    } else {
        cerr << "Write Type should be write-through or write-back" << endl;
        return false;
    }
    return true;
}

bool check_noAlloc_back(string write_func, string through_back) {
    if (write_func.compare("no-write-allocate") == 0 && through_back.compare("write-back") == 0) {
        cerr << "Cannot specify no-write-allocate and write-back together" << endl;
        return false;
    }
    return true;
}

bool check_eviction(Args *vals, string eviction) {
    if (eviction.compare("lru") == 0) {
        vals->lru_state = true;
    } else if (eviction.compare("fifo") == 0) {
        vals->lru_state = false;
    } else {
        cerr << "Evict Type should be lru or fifo" << endl;
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {

    if (argc != 7) { // Check for correct number of arguments
        cerr << argc;
        cerr << "Given Number of Arguments is Invalid!" << endl;
        return 1;
    }
    
    Args vals;
    if (!check_num_sets(&vals, argv[1])) {
        return 1;
    }
    if (!check_num_blocks(&vals, argv[2])) {
        return 1;
    }
    if (!check_num_bytes(&vals, argv[3])) {
        return 1;
    }
    if (!check_write_func(&vals, string(argv[4]))) {
        return 1;
    }
    if (!check_through_back(&vals, string(argv[5]))) {
        return 1;
    }
    if (!check_noAlloc_back(string(argv[4]),string(argv[5]))) {
        return 1;
    }
    if (!check_eviction(&vals, string(argv[6]))) {
        return 1;
    }
   
    CacheSim cache = CacheSim(vals);
    char command;
    uint32_t address;
    uint32_t third; // the third number in the input file line

    // parse each line in trace file; convert address to hexadecimal
    while (cin >> command >> std::hex >> address >> third) {
        // cout << "address: " << address << "\n";
        // cout << "command: " << command << "\n";
        cache.simulate(command, address);
    }

    cache.print_stats(); // print final cache statistics

    return 0;
}