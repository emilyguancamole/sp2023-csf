#include <cstdint>
#include <vector>
#include <map>
#include <set>
#include <list>

using std::vector;
using std::set;
using std::map;

//? structs outside the class?
struct Block {
        uint32_t tag = -1;
        bool valid = false; // checks if loaded into cache
        bool dirty = false; // dirty is for not immediately storing in backing store
        uint32_t time = 0;
};

struct Set {
    vector<Block> blocks;
    // key = tag of the block, value = pointer to Block inside the vector
    map<uint32_t, Block *> block_pointer;
};

struct Args{
    int num_sets;
    int num_blocks_in_set;
    int bytes;
    bool write_thru;
    bool write_alloc;
    bool lru_state;
};

struct Stats{
    int load_hit = 0;
    int load_miss = 0;
    int store_hit = 0;
    int store_miss = 0;
    int tot_loads = 0;
    int tot_stores = 0;
    int tot_cycles = 0;
};


class CacheSim {
private:
    Args vals; // the parameters that are passed in
    Stats stat;
    int cycle_count = 0;

public:
    int num_reads;
    int num_writes;

    vector<Set> sets;

    CacheSim(Args vals); // constructor

    void simulate(char command, uint32_t address);

    void load_block(uint32_t tag, uint32_t index); 
    
    int evict_block(uint32_t index);

    bool block_exists(uint32_t tag, uint32_t index);

};