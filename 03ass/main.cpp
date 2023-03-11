#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string.h>
#include "functions.h"

using std::string;
using std::cout;
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

bool check_num_sets(string numSets) {
    int num_sets;
    if (!isValidNum(numSets)) { //check if the argument is valid
        cerr << "Given Number of Sets is Invalid!" << endl;
        return false;
    } else {
        num_sets = stoi(numSets);
    }
    if (!isPowOfTwo(num_sets)) { // check to ensure sets is a positive power of 2
        cerr << "Given Number of Sets is Not a Power of 2!" << endl;
        return false;
    }
    return true;
}

bool check_num_blocks(string numBlocks) {
    int num_blocks;
    if (!isValidNum(numBlocks)) { //check if the argument is valid
        cerr << "Given Number of Blocks is Invalid!" << endl;
        return false;
    } else {
        num_blocks = stoi(numBlocks);
    }
    if (!isPowOfTwo(num_blocks)) { // check to ensure num of blocks is a positive power of 2
        cerr << "Given Number of Blocks is Not a Power of 2!" << endl;
        return false;
    }
    return true;
}

bool check_num_bytes(string numBytes) {
    int bytes;
    if (!isValidNum(numBytes)) { //check if the argument is valid
        cerr << "Given Number of Bytes is Invalid!" << endl;
        return false;
    } else {
        bytes = stoi(numBytes);
    }
    if (bytes < 4 || !isPowOfTwo(bytes)) { // check to ensure sets is a positive power of 2
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
    if (!check_num_sets(argv[1]) || !check_num_blocks(argv[2]) || !check_num_bytes(argv[3]) || !check_write_func(&vals, string(argv[4]))
        || !check_through_back(&vals, string(argv[5])) || !check_noAlloc_back(string(argv[4]),string(argv[5])) || !check_eviction(&vals, string(argv[6]))) {
        return 1;
    }
    vals.num_sets = stoi(argv[1]);
    vals.num_blocks_in_set = stoi(argv[2]);
    vals.bytes = stoi(argv[3]);
    // vals.write_thru =  stoul(argv[4]);
    // vals.write_alloc = stoul(argv[5]);
    // vals.lru_state =  stoul(argv[6]);
    
    CacheSim cache = CacheSim(vals);

    char command;
    uint32_t address;
    uint32_t trash; // the third number in the input file line

    // parse each line in trace file; convert address to hexadecimal
    while (cin >> command >> std::hex >> address >> trash) { 
        
        cout << "address: " << address << "\n";
        cout << "command: " << command << "\n";
        cache.simulate(command, address);
    }
    return 0;
}