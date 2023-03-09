#include "functions.h"
#include <cstdint>
#include <vector>
#include <set>
#include <algorithm>

using std::set;

// Constructor
CacheSim::CacheSim(int num_sets, int num_blocks_in_set, int bytes, bool write_thru, bool write_alloc, bool lru_state) : 
    num_sets(num_sets), num_blocks_in_set(num_blocks_in_set), bytes(bytes), write_thru(write_thru), write_alloc(write_alloc), lru_state(lru_state) {
    
    load_hit = 0;
    load_miss = 0;
    store_hit = 0;
    store_miss = 0;

    cycles = 0;
    num_sets = 0;
    num_blocks_in_set = 0;

    num_reads = 0;
    num_writes = 0;

    
    int tot_num_blocks = num_blocks_in_set * num_sets;

    

    // initialize all the blocks within each set
    for (int k = 0; k < num_sets; k++) {
        Set set;
        for (int i = 0; i < num_blocks_in_set; i++) {
            Block block;
            block.dirty = false;
            block.valid = false;
            block.tag = -1; // id of block
            block.time = 0;
            set.blocks.insert(block);
        }
        cache.sets.push_back(set);
    }
    
}

void CacheSim::write_through(uint32_t tag, uint32_t index, uint32_t data) {//how to get the data...?
    num_writes++;

}

void CacheSim::write_back(uint32_t tag, uint32_t index, uint32_t data) {

}

void CacheSim::write_allocate() {
    
}

void CacheSim::no_write_allocate() {
}

void CacheSim::lru() {

}

void CacheSim::fifo() {

}

void CacheSim::load_fifo(uint32_t tag, uint32_t index, int bytes) {
    num_reads++;
    
    std::set<Block> cur_set = cache.sets[index].blocks; // index to the current set of blocks

    if (find(tag, index)==true) { // hit
        cycles++;
        load_hit++;
        // update FIFO or LRU (change the order it is stored...?)
    } 
    else { // miss
        load_miss++;
        cycles += (bytes/4) * 100;

        for (auto itr : cur_set) { // find an empty spot and make that block valid
            if (itr.valid == false) { 
                itr.valid = true;
                itr.tag = tag;
                return; 
            }
        }

        // evict based on LRU or FIFO
        // Update everything
        // Loop through the set and see if there is an empty block
        // If there is, store there and if not find LRU block, evict and replace
        // Update FIFO or LRU
    }
}

bool CacheSim::find(uint32_t tag, uint32_t index) { // checks if it's a hit or miss (exist in cache)
    // look at cache at index. then look through set at that index and see if tag matches
    set<Block> cur_set = cache.sets[index].blocks; // index to the current set of blocks
    for (set<Block>::iterator itr = cur_set.begin(); itr != cur_set.end(); itr++) { // iterating through the set at that index
        if (itr->tag == tag) { // find the block with tag -> read hit
            return true;
        } 
    }
    return false;
}