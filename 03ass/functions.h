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
    int miss_count;
    int hit_count;
    int cycles;
    int num_sets;
    int num_blocks_in_set;
    int bytes;

    Cache cache;

    CacheSim(int num_sets, int num_blocks_in_set, int bytes); // constructor

    void write_through(uint32_t tag, uint32_t index);

    void write_allocate();

    void no_write_allocate();

    void write_back();

    void lru();

    void fifo();

    bool isPowOfTwo(int n);

private:
    // helper functions
    void readFromMemory(uint32_t tag, uint32_t index);
};