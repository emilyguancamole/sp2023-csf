#include "functions.h"
#include <cstdint>
#include <vector>

class CacheSim {
public:
    int miss_count;
    int hit_count;
    int cycles;
    int num_sets;
    int num_blocks_in_set;
    int bytes;

    // Constructor
    CacheSim(int num_sets, int num_blocks_in_set, int bytes) : miss_count(0), hit_count(0), cycles(0) {
        Cache cache;
        this->num_sets = num_sets;
        this->num_blocks_in_set = num_blocks_in_set;
        this->bytes = bytes;
        

        int tot_num_blocks = num_blocks_in_set * num_sets;

        // initializing all the blocks within each set
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

    void write_through(uint32_t tag, uint32_t index) {

    }

    void write_allocate() {
        
    }

    void no_write_allocate() {
    }

    

    void write_back() {

    }

    void lru() {

    }

    void fifo() {

    }

};