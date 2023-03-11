#include "functions.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <string.h>

using std::map;
using std::pair;
using std::cout;
using std::cin;
using std::string;

// Constructor
CacheSim::CacheSim(Args vals) {  
    this->vals = vals;
    num_reads = 0;
    num_writes = 0;

    //int tot_num_blocks = vals.num_blocks_in_set * vals.num_sets;

    // initialize all the blocks within each set
    for (int k = 0; k < vals.num_sets; k++) {
        Set set;
        for (int i = 0; i < vals.num_blocks_in_set; i++) {
            Block block;
            set.blocks.push_back(block);
        }
        sets.push_back(set);
    }
}

void CacheSim::simulate() {
    string line;
    string command; //? okay to do str comparisons?
    uint32_t address;
    string address_str;
    uint32_t trash; // the third number in the input file line
    uint32_t offset_size = log2(vals.bytes); // ex. passing 16 would give size=4
    uint32_t index_size = log2(vals.num_blocks_in_set);
    uint32_t tag_size = 32 - offset_size - index_size; 
    uint32_t index;
    uint32_t tag;

    
    while (cin >> command >> address_str >> trash) { // read line by line
        std::stringstream ss; // store address string
        ss << std::hex << address_str.substr(2, 8); // parse as hexadecimal
        ss >> address;

        index = address; // make a copy of address
        index = index << tag_size >> (tag_size + offset_size); // shift to only get index
        tag = address;
        tag = tag >> (index_size + offset_size); // only get tag

        cout << "address: " << address << "\n";
        cout << "tag: " << tag << "\n";
        cout << "index: " << index << "\n";
        
        // some type of switch stmt to process command (s or l)
        if (command == "l") {
            
            // hit: tag at index already exists, so increment stats and don't load
            if (block_exists(tag, index)) { 
                stat.load_hit++;
                stat.tot_cycles++;
                if (vals.lru_state) {
                    sets[index].block_pointer.at(tag)->time = cycle_count;
                }
            } else { // miss, so load block
                stat.load_miss++;
                load_block(tag, index);
            }
            
        } 
        else if (command == "s") {
            
        }
        cycle_count++;
    }
}

void CacheSim::load_block(uint32_t tag, uint32_t index) {
    this->stat.tot_cycles += (this->vals.bytes/4) * 100; // load from memory to cache
    
    vector<Block> cur_set = sets[index].blocks; // get the current vector of blocks at the correct set

    int load_idx = 0; // index to load the new block
    bool full_set = true;
    
    

    for (int idx = 0; idx < this->vals.num_blocks_in_set; idx++) { //looping through set to find an empty place in blocks
        if (cur_set[idx].valid == false) { // empty block
            load_idx = idx; // index of vector of blocks
            full_set = false;
            break;
        }
    }

    if (full_set) { // if the set is full
        load_idx = evict_block(index); // load the new block at this index
    }

    // override the parameters of the new block
    cur_set[load_idx].tag = tag;
    cur_set[load_idx].valid = true;
    cur_set[load_idx].time = cycle_count;
 
    // load the new (tag, block) to the map
    sets[index].block_pointer.insert(pair<uint32_t, Block *>(tag, &cur_set[index]));
}



int CacheSim::evict_block(uint32_t index) { // iterate through all the blocks and check the time stamp to see which block to evict
    vector<Block> cur_set = sets[index].blocks;
    int evict_idx = 0; // index of the block with lowest time stamp
    uint32_t min = UINT32_MAX;

    for (int idx = 0; idx < this->vals.num_blocks_in_set; idx++) {
        if (cur_set[idx].time < min) {
            evict_idx = idx;
        }
    }

    // delete the (tag, block) for the evicted block in the map
    sets[index].block_pointer.erase(cur_set[evict_idx].tag);
    
    return evict_idx;
}

bool CacheSim::block_exists(uint32_t tag, uint32_t index) {
    if (sets[index].block_pointer.find(tag) != sets[index].block_pointer.end()) {
        // tag at index already exists
        return true;
    }
    return false;
}


