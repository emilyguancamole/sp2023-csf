#include <cstdint>
#include <vector>
#include <map>

using std::vector;

//? structs outside the class?
struct Block {
        uint32_t tag;
        bool valid; // checks if loaded into cache
        bool dirty; // dirty is for not immediately storing in backing store
        vector<int> data; //? how to write in bytes in each block
        int lru;
    };

struct Set {
    vector<Block> blocks;
};

struct Cache {
    // index = index, value = block
    vector<Set> sets;
};


class CacheSim {
public:
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

    Cache cache;

    CacheSim(int num_sets, int num_blocks_in_set, int bytes); // constructor

    void write_through(uint32_t tag, uint32_t index, uint32_t data);

    void write_back(uint32_t tag, uint32_t index, uint32_t data);

    void write_allocate();

    void no_write_allocate();

    void lru();

    void fifo();

    bool isPowOfTwo(int n);

    void Load(uint32_t tag, uint32_t index, int bytes);

private:
    // helper functions
    bool find(uint32_t tag, uint32_t index);
};