#include "functions.h"
#include <cstdint>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <algorithm>

using std::set;
using std::map;
using std::pair;

// Constructor
CacheSim::CacheSim(Args vals) {  
    this->vals = vals;
    num_reads = 0;
    num_writes = 0;

    int tot_num_blocks = vals.num_blocks_in_set * vals.num_sets;

    // initialize all the blocks within each set
    for (int k = 0; k < vals.num_sets; k++) {
        Set set;
        for (int i = 0; i < vals.num_blocks_in_set; i++) {
            Block block;
            set.blocks.push_back(block);
        }
        cache.sets.push_back(set);
    }
}

void CacheSim::load_block(uint32_t tag, uint32_t index, int bytes) {
    this->stat.tot_cycles += (this->vals.bytes/4) * 100;
    
    vector<Block> cur_set = cache.sets[index].blocks; // get the current set of blocks at that index

    int load_idx = 0; // index to load the new block
    bool full_set = true;

    for (int idx = 0; idx < this->vals.num_blocks_in_set; idx++) {
        if (cur_set[idx].valid == false) {
            load_idx = idx;
            full_set = false;
            break;
        }
    }

    if (full_set) { // if the set is full
        load_idx = evict_block(tag, index); // load the new block at this index
    }

    // override the parameters of the new block
    cur_set[load_idx].tag = tag;
    cur_set[load_idx].valid = true;
    cur_set[load_idx].time = cycle_count;

    // load the new (tag, block) to the map
    this->cache.sets[index].block_pointer.insert(pair<uint32_t, Block *>(tag, &cur_set[index]));
}

int CacheSim::evict_block(uint32_t tag, uint32_t index) { // iterate through all the blocks and check the time stamp to see which block to evict
    vector<Block> cur_set = cache.sets[index].blocks;
    int evict_idx = 0; // index of the block with lowest time stamp
    uint32_t min = UINT32_MAX;

    for (int idx = 0; idx < this->vals.num_blocks_in_set; idx++) {
        if (cur_set[idx].time < min) {
            evict_idx = idx;
        }
    }

    // delete the (tag, block) for the evicted block in the map
    this->cache.sets[index].block_pointer.erase(cur_set[evict_idx].tag);
    
    return evict_idx;
}

