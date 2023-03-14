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

    // initialize all the blocks within each set
    for (int k = 0; k < vals.num_sets; k++) {
        Set s;
        s.filled_count = 0;
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
    uint32_t index_size = log2(vals.num_blocks_in_set);

    uint32_t index = 0; // initialize to 0
    uint32_t tag_size = 32 - offset_size - index_size; 
    if (index_size != 0) { 
        index = (address << tag_size) >> (32-index_size); // shift to only get index
    }
    uint32_t tag = address >> (index_size + offset_size); // get tag
    
    if (command == 'l') {
        load_block(tag, index);
        stat.tot_loads++;
    }  else if (command == 's') {
        if (vals.write_thru) { // write-through store writes to the cache as well as to memory
            stat.tot_cycles += 100;
        }
        stat.tot_stores++;
        store_block(tag, index);
    }
    stat.tot_cycles++; // increment cycles for each load/store instr bc dealing with cache

    cycle_count++; // increment count for each line of trace file
}

void CacheSim::load_block(uint32_t tag, uint32_t index) {
    // store hit: tag at index already exists, so increment stats and don't load
    if (block_exists(tag, index)) { 
        stat.load_hit++;
        sets[index].block_pointer.at(tag)->time_lru = cycle_count;
        
    } else { // load miss, so load block from memory
        stat.load_miss++;
        stat.tot_cycles += (vals.bytes/4) * 100; //? load from memory to cache

        load_miss(tag, index); // load new block
        // if (!vals.write_thru) {
        //     sets[index].block_pointer[tag]->dirty = false; // reset to dirty
        // }
    }
}

void CacheSim::store_block(uint32_t tag, uint32_t index) {
    if (block_exists(tag, index)) { // store hit
        (stat.store_hit)++;
        if (!vals.write_thru) { // write-back writes to the cache only and marks the block dirty
            sets[index].block_pointer.at(tag)->dirty = true;
        }
        //if (vals.lru_state) { // update timestamp for lru
        sets[index].block_pointer.at(tag)->time_lru = cycle_count; 
        //}
    } else { // store miss
        (stat.store_miss)++; 
        if (vals.write_alloc) { // write alloc: load block from memory into cache
            load_miss(tag, index);
            if (!vals.write_thru) { // if write back, set dirty to true
               // sets[index].block_pointer.at(tag)->dirty = true;
            }
            stat.tot_cycles += (vals.bytes/4) * 100; //? need to make sure cycles are updated by /4*100 every time // update cycles from loading from memory
        }
    }
}

void CacheSim::load_miss(uint32_t tag, uint32_t index) {
    //vector<Block> cur_set = sets[index].blocks; // get the current vector of blocks at the correct set
    //int load_idx = find_load_idx(tag, index); // index to load the new block

    bool full_set = true;
    Block* updated; // pointer to block to be updated
    
    int filled = sets[index].filled_count;
    if (filled < vals.num_blocks_in_set) {
        updated = &sets[index].blocks[filled]; // last empty block
        sets[index].filled_count++;
    } else { // all blocks are filled
        int evict_idx = evict_block(index);

        updated = &sets[index].blocks[evict_idx]; // set updated to address of block to be evicted
        sets[index].block_pointer.erase(updated->tag);
        // if write-back and block is dirty, need to write to memory
        if (!vals.write_thru && updated->dirty /*?sets[index].blocks[load_idx].dirty*/) {
            updated->dirty = false; // reset dirty
            this->stat.tot_cycles += (vals.bytes/4) * 100;
        }
    }

    // // loop through set to find an empty place in blocks
    // for (int k = 0; k < vals.num_blocks_in_set; k++) { 
    //     if (sets[index].blocks[k].valid == false) { // found an empty block to replace
    //         updated = &sets[index].blocks[k]; // return the index and break out of the function
    //         full_set = false;
    //         break;
    //     }
    // }
    // override the parameters of the new block
    updated->tag = tag;
    updated->valid = true;
    updated->time_lru = cycle_count;
    updated->time_fifo = cycle_count;
    // sets[index].blocks[load_idx].tag = tag;
    // sets[index].blocks[load_idx].valid = true;
    // sets[index].blocks[load_idx].time_lru = cycle_count;
    // sets[index].blocks[load_idx].time_fifo = cycle_count;
    
    // load the new (tag, block*) to the map, so the new location in map points to new block
    sets[index].block_pointer.insert({tag, updated});
}

// // finds index of empty spot in blocks if found. If not found, return -1.
// int CacheSim::find_load_idx(uint32_t tag, uint32_t index) {
//     int load_idx = -1; // index to load the new block
    
//     //// if reached here, must evict based on lru or fifo
//     //load_idx = evict_block(index); // load the new block at the index of evicted block

//     return load_idx; // not found, meaning blocks is full
// }

int CacheSim::evict_block(uint32_t index) { // iterate through all the blocks and check the time stamp to see which block to evict
    int evict_idx = 0; // index of the block with lowest time stamp
    uint32_t min_time = UINT32_MAX;

    // initialize min_time with first time value??
    // if (vals.lru_state) min_time = sets[index].blocks[0].time_lru;
    // else min_time = sets[index].blocks[0].time_fifo;

    for (int k = 0; k < vals.num_blocks_in_set; k++) {
        if (vals.lru_state && sets[index].blocks[k].time_lru < min_time) { // find lowest lru time
            evict_idx = k;
            min_time = this->sets[index].blocks[k].time_lru; // new minimum time
        } else if (!vals.lru_state && sets[index].blocks[k].time_fifo < min_time) { // find lowest fifo time
            evict_idx = k;
            min_time = this->sets[index].blocks[k].time_fifo; // new minimum time
        }
    }
    return evict_idx;
}

bool CacheSim::block_exists(uint32_t tag, uint32_t index) {
    return (sets.at(index).block_pointer.find(tag) != sets.at(index).block_pointer.end());
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