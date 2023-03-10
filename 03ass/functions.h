#include <cstdint>
#include <vector>
#include <map>
#include <set>
#include <list>

using std::vector;

//? structs outside the class?
struct Block {
        uint32_t tag;
        bool valid; // checks if loaded into cache
        bool dirty; // dirty is for not immediately storing in backing store
        vector<int> data; //? how to write in bytes in each block
        int time;
    };

struct Set {
    std::set<Block> blocks;
};

struct Cache {
    // index = index, value = block
    vector<Set> sets;
};


class CacheSim {
public:
    bool write_thru;
    bool write_alloc;
    bool lru_state;
    int load_hit;
    int load_miss;
    int store_hit;
    int store_miss;
    
    int cycles;
    int num_sets;
    int num_blocks_in_set;
    int bytes;

    int num_reads;
    int num_writes;

    std::list<Block> fifo_list;

    Cache cache;

    CacheSim(int num_sets, int num_blocks_in_set, int bytes, bool write_thru, bool write_alloc, bool lru_state); // constructor

    void write_through(uint32_t tag, uint32_t index, uint32_t data);

    void write_back(uint32_t tag, uint32_t index, uint32_t data);

    void write_allocate(uint32_t tag, uint32_t index, int bytes, int num_blocks_in_set, bool lru_state);

    void no_write_allocate();

    void lru();

    void fifo();

    void store(uint32_t tag, uint32_t index, int bytes, int num_blocks_in_set, bool lru_state);

    void load(uint32_t tag, uint32_t index, int bytes, int num_blocks_in_set, bool lru_state);

private:
    set<Block>::iterator CacheSim::find(uint32_t tag, uint32_t index);
    //void set_block(Block &b, uint32_t tag, bool dirty=false, bool valid=true);
};