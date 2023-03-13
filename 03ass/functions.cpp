#include "functions.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <sys/types.h>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <string.h>

using std::map;
using std::pair;
using std::cout;
using std::string;
using std::endl;

// Constructor
CacheSim::CacheSim(Args vals) {  
    this->vals = vals;
    num_reads = 0;
    num_writes = 0;
    cycle_count = 0;

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

void CacheSim::simulate(char command, uint32_t address) {

    uint32_t offset_size = log2(vals.bytes); // ex. passing 16 would give size=4
    //cout << offset_size << endl;
    uint32_t index_size = log2(vals.num_blocks_in_set);
    //cout << index_size << endl;

    uint32_t index = 0; // initialize to 0
    uint32_t tag_size = 32 - offset_size - index_size; 
    if (index_size != 0) { 
        index = (address << tag_size) >> (32-index_size); // shift to only get index
    }
    uint32_t tag = address >> (index_size + offset_size); // get tag
    
    this->stat.tot_cycles++; // increment cycles for each load/store instr bc dealing with cache

    if (command == 'l') {
        this->stat.tot_loads++;
        load_block(tag, index);
    }  else if (command == 's') {
        stat.tot_stores++;
        if (vals.write_thru) { // write-through store writes to the cache as well as to memory
            stat.tot_cycles += 100;
        }

        store_block(tag, index);
        
    }
    this->cycle_count++; // increment count for each line of trace file
}

void CacheSim::load_block(uint32_t tag, uint32_t index) {
    // store hit: tag at index already exists, so increment stats and don't load
    if (block_exists(tag, index)) { 
        stat.load_hit++;
        if (vals.lru_state) {
            sets[index].block_pointer.at(tag)->time = cycle_count;
        }
    } else { // load miss, so load block from memory
        stat.load_miss++; //? need to make sure cycles are updated by /4*100 every time
        stat.tot_cycles += (vals.bytes/4) * 100; // load from memory to cache

        update_block(tag, index);
    }
}

void CacheSim::store_block(uint32_t tag, uint32_t index) {
    if (block_exists(tag, index)) { // store hit
        (stat.store_hit)++;
        //this->stat.tot_cycles++; // found the block, so stores to/from cache take 1 cycle
        if (!vals.write_thru) { // write-back writes to the cache only and marks the block dirty
            sets[index].block_pointer.at(tag)->dirty = true;
        }
        if (vals.lru_state) { // update timestamp for lru
            sets[index].block_pointer.at(tag)->time = cycle_count++; 
        }
    } else { // store miss
        (stat.store_miss)++; 
        if (vals.write_alloc) { // write alloc: load block from memory into cache
            update_block(tag, index);
            if (!vals.write_thru) { // if write back, set dirty to true
                //this->stat.tot_cycles++; //?? i think we do this in load_block
                this->sets[index].block_pointer.at(tag)->dirty = true;
            }
            stat.tot_cycles += (vals.bytes/4) * 100; //? need to make sure cycles are updated by /4*100 every time // update cycles from loading from memory
        }
    }
}
void CacheSim::update_block(uint32_t tag, uint32_t index) {
    //vector<Block> cur_set = sets[index].blocks; // get the current vector of blocks at the correct set
    int load_idx = 0; // index to load the new block
    bool full_set = true;
    
    // loop through set to find an empty place in blocks
    for (int idx = 0; idx < this->vals.num_blocks_in_set; idx++) { 
        if (sets[index].blocks[idx].valid == false) { // empty block
            load_idx = idx; // index of vector of blocks
            full_set = false;
            break;
        }
    }

    if (full_set) { // if the set is full
        load_idx = evict_block(index); // load the new block at the index of evicted block
    }

    // override the parameters of the new block
    sets[index].blocks[load_idx].tag = tag;
    sets[index].blocks[load_idx].valid = true;
    sets[index].blocks[load_idx].time = cycle_count++;

    // load the new (tag, block) to the map, so the new location in map points to new block
    sets[index].block_pointer.insert(pair<uint32_t, Block *>(tag, &(sets[index].blocks[load_idx])));
}

int CacheSim::evict_block(uint32_t index) { // iterate through all the blocks and check the time stamp to see which block to evict
    //vector<Block> cur_set = this->sets[index].blocks;
    int evict_idx = 0; // index of the block with lowest time stamp
    uint32_t min = UINT32_MAX;

    for (int idx = 0; idx < this->vals.num_blocks_in_set; idx++) {
        if (sets[index].blocks[idx].time < min) {
            evict_idx = idx;
        }
    }
    // if write-back and block is dirty, need to write to memory before evicting
    if (!vals.write_thru && sets[index].blocks[evict_idx].dirty) {
        this->stat.tot_cycles += (this->vals.bytes/4) * 100;
    }

    // delete the (tag, block) for the evicted block in the map
    this->sets[index].block_pointer.erase(sets[index].blocks[evict_idx].tag);
    
    return evict_idx;
}

bool CacheSim::block_exists(uint32_t tag, uint32_t index) {
    map<uint32_t, Block*>::iterator itr = sets[index].block_pointer.find(tag);

    if (itr != sets[index].block_pointer.end()){
        return true;
    } else {
        return false;
    }
}

void CacheSim::print_stats() {
  cout << "Total loads: " << stat.tot_loads << endl;
  cout << "Total stores: " << stat.tot_stores << endl;
  cout << "Load hits: " << stat.load_hit << endl;
  cout << "Load misses: " << stat.load_miss << endl;
  cout << "Store hits: " << stat.store_hit << endl;
  cout << "Store misses: " << stat.store_miss << endl;
  cout << "Total cycles: " << stat.tot_cycles << endl;
}