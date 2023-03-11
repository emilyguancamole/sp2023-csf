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
        int time = 0;
};

struct Set {
    vector<Block> blocks;
    // key = tag of the block, value = pointer to Block inside the vector
    map<uint32_t, Block *> block_pointer;
};

struct Cache {
    vector<Set> sets;
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
    Args vals;
    Stats stat;
    int cycle_count = 0;
    set<Block>::iterator CacheSim::find(uint32_t tag, uint32_t index);

public:
    int num_reads;
    int num_writes;

    Cache cache;

    CacheSim(Args vals); // constructor

    void load_block(uint32_t tag, uint32_t index, int bytes); 
    
    int evict_block(uint32_t tag, uint32_t index);

};