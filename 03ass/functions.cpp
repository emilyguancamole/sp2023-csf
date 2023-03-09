#include "functions.h"
#include <cstdint>
#include <vector>
#include <set>
#include <list>
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

void CacheSim::write_through(uint32_t tag, uint32_t index, uint32_t data) {//? how to get the data...?
    num_writes++;
    cycles += (bytes/4) * 100; // write to memory
    // write to cache
    load_fifo(tag, index, bytes); // call helper
    //? wait what's the diff between writing to memory vs. cache
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
    } else { // miss
        load_miss++;
        cycles += (bytes/4) * 100;

        for (auto itr : cur_set) { // find an empty spot and make that block valid
            if (itr.valid == false) { 
                itr.dirty = false;
                itr.valid = true;
                itr.tag = tag;
                fifo_list.push_back(itr); // block was last recently loaded
                return; 
            }
        }
        // the cache was full, so replace first-in block with new block 
        Block evicted = fifo_list.front(); //? stores a copy of the first element
        fifo_list.pop_front(); // remove first-in block
        evicted.dirty = false; //? reset the block, making it a new block
        evicted.valid = true;
        evicted.tag = tag;
        ////set_block(&evicted, tag); //? lol pass by ref
        fifo_list.push_back(evicted); // add to fifo list to track when block was made
        
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

// inialize new block with properties
void CacheSim::set_block(Block &b, uint32_t tag, bool dirty=false, bool valid=true) {
    b.dirty = dirty;
    b.valid = valid;
    b.tag = tag;
}