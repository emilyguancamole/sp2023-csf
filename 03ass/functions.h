#include <vector>

using std::vector;

struct Block {
    int tag;
    bool dirty; // dirty is for not immediately storing in backing store
    vector<int> data;
};

struct Set {
    vector<Block> blocks;
};

struct Cache {
    vector<Set> blocks;
};

void write_allocate();

void no_write_allocate();

void write_through();

void write_back();

void lru();

void fifo();