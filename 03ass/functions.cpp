#include "functions.h"
#include <cstdint>
#include <vector>
#include <algorithm>

// Constructor
CacheSim::CacheSim(int num_sets, int num_blocks_in_set, int bytes) : 
    num_sets(num_sets), num_blocks_in_set(num_blocks_in_set), bytes(bytes) {

    miss_count = 0;
    hit_count = 0;
    cycles = 0;
    num_sets = 0;
    num_blocks_in_set = 0;

    
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

void CacheSim::write_through(uint32_t tag, uint32_t index) {
    
}

void CacheSim::write_allocate() {
    
}

void CacheSim::no_write_allocate() {
}


void CacheSim::write_back() {

}

void CacheSim::lru() {

}

void CacheSim::fifo() {

}

void CacheSim::readFromMemory(uint32_t tag, uint32_t index) {
    // look at cache at index. then look through set at that index and see if tag matches
    vector<Block> cur_set = cache.sets[index].blocks; // index to the current set of blocks
    for (unsigned int i = 0; i < cur_set.size(); i++) {
        if (cur_set[i].tag == tag) {
            // hit
        }
    }
}