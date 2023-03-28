/*
 * Declaration of CacheSim, class for a cache simulator that mimics loads, stores, and cycles of a cache,
 * along with properties to specify cache configuration and statistics.
 * CSF Assignment 3
 * Emily Guan, eguan2
 * Esther Kwon, 
 */
#include <cstdint>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <string>

using std::vector;
using std::map;
using std::string;

/*
* Block struct storing properties of a block/slot within the cache.
*/
struct Block {
    uint32_t tag = 0;
    bool valid = false; // checks if loaded into cache
    bool dirty = false; // dirty is for not immediately storing in backing store
    uint32_t time_lru = 0;
    uint32_t time_fifo = 0;
};

/*
* Set struct storing a vector of Blocks and map to locate each block.
*/
struct Set {
    vector<Block> blocks;
    map<uint32_t, Block *> block_pointer; // key = tag of the block, value = pointer to Block inside the vector
};

/*
* Args struct storing arguments that specify a particular cache configuration.
*/
struct Args{
    int num_sets = 0;
    int num_blocks_in_set = 0;
    int bytes = 0;
    bool write_thru = false;
    bool write_alloc = false;
    bool lru_state = false;
};

/*
* Stats struct storing statistics for the cache simulator to be outputted.
*/
struct Stats{
    int load_hit = 0;
    int load_miss = 0;
    int store_hit = 0;
    int store_miss = 0;
    int tot_loads = 0;
    int tot_stores = 0;
    int tot_cycles = 0;
};

/*
* Definition of CacheSim class. Simulates loads and stores of a cache for a specified configuration.
*/
class CacheSim {
private:
    Args vals; // the parameters that are passed in
    int cycle_count; // counting number of lines in the trace file, used for timestamp
    vector<Set> sets;

public:
    int num_reads;
    int num_writes;
    Stats stat;
    /*
    * CacheSim constructor; initializes stats to 0 and initializes all Blocks in each set
    * 
    * Parameters:
    *   vals - 
    */
    CacheSim(Args vals);

    /*
    * Carries out cache simulation. Calculates index, handles loads, stores, and updating statistics.
    * 
    * Parameters:
    *  command - specifies l for load, s for store
    *  address - memory address in hexadecimal
    */
    void simulate(char command, uint32_t address);

    void load_block(uint32_t tag, uint32_t index);

    void store_block(uint32_t tag, uint32_t index);

    void load_miss(uint32_t tag, uint32_t index);
    //void load_block(uint32_t tag, uint32_t index); 

    int find_load_idx(uint32_t tag, uint32_t index);
    
    int evict_block(uint32_t index);

    bool block_exists(uint32_t tag, uint32_t index);

    void print_stats();

};