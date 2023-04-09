/*
 * Implementation of CacheSim, which mimics loads, stores, and cycles of a cache.
 * CSF Assignment 3
 * Emily Guan, eguan2
 * Esther Kwon, kkwon16
*/
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

using std::pair;
using std::cout;
using std::string;
using std::endl;

// Constructor
CacheSim::CacheSim(Args vals) {  
    // initialize values that specify configuration
    this->vals = vals;
    cycle_count = 0;

    // initialize all the blocks within each set
    for (int k = 0; k < vals.num_sets; k++) {
        Set s;
        for (int i = 0; i < vals.num_blocks_in_set; i++) {
            Block block;
            s.blocks.push_back(block);
        }
        this->sets.push_back(s);
    }
}

void CacheSim::simulate(char command, uint32_t address) {
    // calculate offset and index size for shifting the address
    uint32_t offset_size = log2(vals.bytes); 
    uint32_t index_size = log2(vals.num_sets);

    uint32_t index = 0; // initialize to 0
    uint32_t tag_size = 32 - offset_size - index_size; 
    if (index_size != 0) { 
        index = (address << tag_size) >> (32-index_size); // shift to only get index
    }
    uint32_t tag = address >> (index_size + offset_size); // get tag

    if (command == 'l') { // deal with load
        this->stat.tot_loads++;
<<<<<<< HEAD
        load_block(tag, index); // load block into cache
    }  else if (command == 's') {
=======
        load_block(tag, index);
    }  else if (command == 's') { // deal with store
>>>>>>> b68eb3fb17545ecbc8ded90b5ca8d71b6937af3a
        this->stat.tot_stores++;
        if (this->vals.write_thru) { // write-through store writes to the cache as well as to memory
            stat.tot_cycles += 100;
        }
        store_block(tag, index); // call store block
    }
    this->cycle_count++; // increment count for each line of trace file
}

void CacheSim::load_block(uint32_t tag, uint32_t index) {
    // store hit: tag at index already exists, so increment stats and don't load
    if (block_exists(tag, index)) { 
        this->stat.tot_cycles++;
        this->stat.load_hit++;
        if (this->vals.lru_state) {
            sets[index].block_pointer.at(tag)->time_lru = cycle_count; // update time stamp for lru
        }
    } else { // load miss, so load block from memory
        this->stat.load_miss++;
        load_miss(tag, index); // load new block
    }
}

void CacheSim::store_block(uint32_t tag, uint32_t index) {
    if (block_exists(tag, index)) { // store hit
        (stat.store_hit)++;
        this->stat.tot_cycles++;
        if (!vals.write_thru) { // write-back writes to the cache only and marks the block dirty
            sets[index].block_pointer.at(tag)->dirty = true;
        }
        if (vals.lru_state) { // update timestamp for lru
            sets[index].block_pointer.at(tag)->time_lru = cycle_count; 
        }
    } else { // store miss
        (stat.store_miss)++; 
        if (vals.write_alloc) { // write alloc: load block from memory into cache
            load_miss(tag, index);
            if (!vals.write_thru) { // if write back, set dirty to true
                sets[index].block_pointer.at(tag)->dirty = true;
            }
        }
    }
}

void CacheSim::load_miss(uint32_t tag, uint32_t index) {
    this->stat.tot_cycles += ((this->vals.bytes/4) * 100); // update cycle for the miss - add the one for cycle count

    bool full_set = true; // initially, set flag to true
    vector<Block> *cur_set = &this->sets.at(index).blocks; // get the set at given index

    int new_idx = 0;

    // loop through set to find an empty place in blocks
    for (int k = 0; k < vals.num_blocks_in_set; k++) { 
        if (!cur_set->at(k).valid) { // found an empty block to replace
            new_idx = k; // return the index and break out of the function
            full_set = false; // set is not full
            break;
        }
    }

    if (full_set) { // if the cache is full, need to look for block with lowest lru or fifo
        new_idx = evict_block(index);
    }

    // reset parameters for the replaced block
    cur_set->at(new_idx).tag = tag;
    cur_set->at(new_idx).valid = true;

    // update time stamp depending on the type of eviction
    if (this->vals.lru_state) {
        cur_set->at(new_idx).time_lru = cycle_count;
    } else {
        cur_set->at(new_idx).time_fifo = cycle_count;
    }

    // load the new (tag, block*) to the map, so the new location in map points to new block
    this->sets.at(index).block_pointer.insert(pair<uint32_t, Block *>(tag, &cur_set->at(new_idx)));
}

int CacheSim::evict_block(uint32_t index) { // iterate through all the blocks and check the time stamp to see which block to evict
    int evict_idx = 0; // index of the block with lowest time stamp
    vector<Block> *cur_set = &this->sets.at(index).blocks;
<<<<<<< HEAD
    // compare timestamp depending on eviction type
    if (this->vals.lru_state) { // lru eviction
=======

    // compare timestamp depending on eviction type
    if (this->vals.lru_state) { 
>>>>>>> b68eb3fb17545ecbc8ded90b5ca8d71b6937af3a
        for (int i = 1; i < vals.num_blocks_in_set; i++) {
            if (cur_set->at(i).time_lru < cur_set->at(evict_idx).time_lru) { // iterate through and find the block with lowest time stamp
                evict_idx = i; // replace index if a lower timestamp is found
            }
        }
    } else { // fifo eviction
        for (int j = 1; j < vals.num_blocks_in_set; j++) {
            if (cur_set->at(j).time_fifo < cur_set->at(evict_idx).time_fifo) {
                evict_idx = j;
            }
        }
    }

    if (cur_set->at(evict_idx).dirty) { // if the block was dirty, update cycle before eviction
        this->stat.tot_cycles += ((this->vals.bytes/4) * 100);
    }

    this->sets.at(index).block_pointer.erase(cur_set->at(evict_idx).tag); // erase the block at that tag

    return evict_idx; // return index for new block to be inserted
}

bool CacheSim::block_exists(uint32_t tag, uint32_t index) { // check if the block exists
    return (this->sets.at(index).block_pointer.find(tag) != this->sets.at(index).block_pointer.end());
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