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
 /*
void CacheSim::write_through(uint32_t tag, uint32_t index, uint32_t data) {//? how to get the data...?
    num_writes++;
    cycles += (bytes/4) * 100; // write to memory
    // write to cache
    load(tag, index, bytes, num_blocks_in_set, write_thru); // call helper
    //? what's the diff between writing to memory vs. cache
    //? diff between load and store -> what are we doing in load_fifo

    // go to cache[index].blocks
    // add block to a slot in there
}
 */


void CacheSim::write_back(uint32_t tag, uint32_t index, uint32_t data) {
    //What to do...???
}

void CacheSim::write_allocate(uint32_t tag, uint32_t index, int bytes, int num_blocks_in_set, bool lru_state) {
    load(tag, index, bytes, num_blocks_in_set, lru_state);

}

void CacheSim::no_write_allocate() {
}

void CacheSim::lru() {

}

void CacheSim::fifo() {

}

void CacheSim::store(uint32_t tag, uint32_t index, int bytes, int num_blocks_in_set, bool lru_state) {
    num_writes++;

    set<Block> cur_set = cache.sets[index].blocks;
    set<Block>::iterator target_itr = find(tag, index);

    if (target_itr != cur_set.end()) {
        if (lru_state) {
            Block target = *(target_itr);
            target.time = num_writes;
        }
        cycles++;
        store_hit++;
    } else {
        store_miss++;
        cycles += (bytes/4) * 100;

        load(tag, index, bytes, num_blocks_in_set, lru_state);
    }
}

void CacheSim::load(uint32_t tag, uint32_t index, int bytes, int num_blocks_in_set, bool lru_state) {
    num_reads++;
    
    set<Block> cur_set = cache.sets[index].blocks; // index to the current set of blocks
    set<Block>::iterator target_itr = find(tag, index);
    
    if (target_itr != cur_set.end()) { // hit
        if (lru_state) { // need to update time stamp for lru
            Block target;
            target = *(target_itr);
            target.time = num_reads;
        }
        cycles++;
        load_hit++;

    } else { // miss
        load_miss++;
        cycles += (bytes/4) * 100;

        Block target;
        Block replace;
        int min_lru = INT32_MAX;

        if (cur_set.size() == num_blocks_in_set) { // if the set is full
            for (target_itr = cur_set.begin(); target_itr != cur_set.end(); target_itr++) { // iterate through to find block with lowest lru value
                if (target_itr->time < min_lru) {
                    target = *(target_itr);
                }
            }
            cur_set.erase(target); // erase the block from set
        }

        if (!lru_state) { // need to set block to dirty....????????/
            replace.dirty = true;
        }

        replace.valid = true; // create and set a new block
        replace.tag = tag;
        replace.time = num_reads;
        cur_set.insert(replace); // insert into the set
    }
}


 set<Block>::iterator CacheSim::find(uint32_t tag, uint32_t index) { // checks if it's a hit or miss (exist in cache)
    // look at cache at index. then look through set at that index and see if tag matches
    set<Block> cur_set = cache.sets[index].blocks; // index to the current set of blocks
    set<Block>::iterator itr;
    for (itr = cur_set.begin(); itr != cur_set.end(); itr++) { // iterating through the set at that index
        if (itr->tag == tag) { // find the block with tag -> read hit
            return itr;
        } 
    }
    return cur_set.end();
}

/*
// inialize new block with properties
void CacheSim::set_block(Block &b, uint32_t tag, bool dirty, bool valid) {
    b.dirty = dirty;
    b.valid = valid;
    b.tag = tag;
}
*/
