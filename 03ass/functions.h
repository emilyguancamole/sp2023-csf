#include <cstdint>
#include <vector>
#include <map>

using std::vector;

struct Block {
    int tag;
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

void write_allocate();

void no_write_allocate();

void write_through();

void write_back();

void lru();

void fifo();


bool isPowOfTwo(int n);