#include "functions.h"
#include <cstdint>
#include <vector>
#include <algorithm>

// Constructor
CacheSim::CacheSim(int num_sets, int num_blocks_in_set, int bytes) : 
    num_sets(num_sets), num_blocks_in_set(num_blocks_in_set), bytes(bytes) {
    
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
            block.lru = 0;
            set.blocks.push_back(block);
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

void CacheSim::Load(uint32_t tag, uint32_t index, int bytes) {
    num_reads++;
    
    vector<Block> cur_set = cache.sets[index].blocks; // index to the current set of blocks

    if (find(tag, index)) {
        cycles++;
        load_hit++;
        // update FIFO or LRU (change the order it is stored...?)
    } else {
        load_miss++;
        cycles += (bytes/4) * 100;

        for (unsigned int i = 0; i < cur_set.size(); i++) {
            if (cur_set[i].valid == false) {
                cur_set[i].valid = true;
                cur_set[i].dirty = false;
                cur_set[i].tag = tag;
                cur_set[i].lru = num_reads;
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
    vector<Block> cur_set = cache.sets[index].blocks; // index to the current set of blocks
    for (unsigned int i = 0; i < cur_set.size(); i++) { // iterating through the set at that index
        if (cur_set[i].tag == tag) { // find the block with tag -> read hit
            return true;
        } 
    }
    return false;
}