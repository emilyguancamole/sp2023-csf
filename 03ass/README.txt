Esther Kwon, Emily Guan
We worked on all the functions equally and went to office hours together. Met in person to write the code together.
For running the tests, we divided it up into finding the best write policy (write-allocate + write-back) & associativity
and lru/fifo & block size. Emily completed the first parts and Esther completed the latter half - discussed our results
together at the end to finalize.

To determine the cache configuration with the best overall effectiveness, we ran experiments with the gcc.trace
file for each possible cache configuration. That is, we tested direct-mapped, fully associative, and set associative
configurations.

We tested write policies: (write-allocate + write-back), (write-allocate + write-through), and 
(no-write-allocate + write-through) for each configuration.

For each combination of configuration and the above parameters, we repeated the tests with lru and fifo 
(fifo was not done with direct-mapping, as eviction type does not matter).

The number of sets and blocks per set were changed to alter the cache configuration 
(i.e. direct-map has n sets of 1 block, fully associative cache has 1 set of n blocks).
However, the total cache capacity was kept at 2^14 = 16,384 bytes. Each block consistently contained 16 bytes of memory.

***** EXPERIMENTS *****
The commands to simulate each experiment, along with results, are summarized below.

*** write-allocate + write-back ***
** lru **
Direct-mapped (1 block per set):
./csim 1024 1 16 write-allocate write-back lru < gcc.trace
    Load hits: 312238
    Load misses: 5959
    Store hits: 187502
    Store misses: 9984
    Total cycles: 12626940
Fully associative (1 set):
./csim 1 1024 16 write-allocate write-back lru < gcc.trace
    Load hits: 314973
    Load misses: 3224
    Store hits: 188300
    Store misses: 9186
    Total cycles: 9756473
Set-associative (256 sets, 4 blocks each):
./csim 256 4 16 write-allocate write-back lru < gcc.trace
    Load hits: 314798
    Load misses: 3399
    Store hits: 188250
    Store misses: 9236
    Total cycles: 9966648
** fifo **
Fully associative:
./csim 1 1024 16 write-allocate write-back fifo < gcc.trace
    Load hits: 314311
    Load misses: 3886
    Store hits: 188117
    Store misses: 9369
    Total cycles: 10419228
Set-associative:
./csim 256 4 16 write-allocate write-back fifo < gcc.trace
    Load hits: 314171
    Load misses: 4026
    Store hits: 188047
    Store misses: 9439
    Total cycles: 10628218


*** write-allocate + write-through ***
** lru **
Direct-mapped:
./csim 1024 1 16 write-allocate write-through lru < gcc.trace
    Load hits: 312238
    Load misses: 5959
    Store hits: 187502
    Store misses: 9984
    Total cycles: 26625540
Fully associative:
./csim 1 1024 16 write-allocate write-through lru < gcc.trace
    Load hits: 314973
    Load misses: 3224
    Store hits: 188300
    Store misses: 9186
    Total cycles: 25215873
Set-associative:
./csim 256 4 16 write-allocate write-through lru < gcc.trace
    Load hits: 314798
    Load misses: 3399
    Store hits: 188250
    Store misses: 9236
    Total cycles: 25305648
** fifo **
Fully associative:
    Load hits: 314311
    Load misses: 3886
    Store hits: 188117
    Store misses: 9369
    Total cycles: 25553028
Set-associative:
    Load hits: 314171
    Load misses: 4026
    Store hits: 188047
    Store misses: 9439
    Total cycles: 25636818


*** no-write-allocate + write-through ***
** lru **
Direct-mapped:
./csim 1024 1 16 no-write-allocate write-through lru < gcc.trace
    Load hits: 309108
    Load misses: 9089
    Store hits: 162581
    Store misses: 34905
    Total cycles: 23855889
Fully associative:
./csim 1 1024 16 no-write-allocate write-through lru < gcc.trace
    Load hits: 311723
    Load misses: 6474
    Store hits: 164778
    Store misses: 32708
    Total cycles: 22814701
Set-associative:
./csim 256 4 16 no-write-allocate write-through lru < gcc.trace
    Load hits: 311613
    Load misses: 6584
    Store hits: 164819
    Store misses: 32667
    Total cycles: 22858632
** fifo **
Fully associative:
./csim 1 1024 16 no-write-allocate write-through fifo < gcc.trace
    Load hits: 311160
    Load misses: 7037
    Store hits: 164196
    Store misses: 33290
    Total cycles: 23038756
Set-associative:
./csim 256 4 16 no-write-allocate write-through fifo < gcc.trace
    Load hits: 311017
    Load misses: 7180
    Store hits: 163705
    Store misses: 33781
    Total cycles: 23095322


***** ANALYSIS *****

When comparing across cache mapping techniques (direct mapping vs. fully asssociative vs. set associative),
the main difference is seen in total cycle count. 
    For (write-allocate + write-back) & lru, direct-mapping had about 2 million more total cycles than associative mapping. 
    For (write-allocate + write-through) & lru, direct mapping had over 1.3 million more total cycles than associative mapping.
    For (no-write-allocate + write-through) & lru, direct mapping had about 1 million more total cycles (the smallest difference
    among lru eviction).
    The other statistics were not significantly different. 

    This analysis is generally true for both lru and fifo eviction. However, we did notice that differences
    in total cycles for fifo eviction were smaller than those for lru eviction. The smallest of these 
    differences was seen in (no-write-allocate + write-through): direct mapping had about 800 thousand more cycles.
    From this experiment, we can conclude that set-associative mapping is the most effective as it balances the advantages
    of direct mapping and fully associative mapping, reducing the likelihood of cache conflicts and improving cache hit rates.
    Although the total cycle count is smaller for fully associative mapping, this method is not feasible in real life
    due to its high hardware requirements, increased latency, and difficulty in managing large amount of data.


We then compared results between write policies, keeping other parameters of each cache constant. 

    Compared to write-through, write-back had significantly fewer total cycles. 
    For instance, for set-assoc & lru, we saw a difference of 13-15 million cycles.
                        write-back   (write-alloc +) write-through  (no-write-alloc +) write-through
        Total cycles:   9966648      25305648                       22858632

    Compared to write-allocate, no-write-allocate had more load misses 
    For instance, direct mapping & lru:
                        write-alloc    no-write-alloc
        Load misses:    5959           9089     
        Store misses:   9984           34905
        high adssoc factor & higher block size gives more effect: miss rate dec, tot cycles inc

    Therefore, write-back + write-allocate was the most effective combination of write policies.


We did further comparisons of results for lru vs. fifo eviction.
    Overall, lru eviction was slightly more efficient than fifo eviction, as it had lower miss rates (differences of several hundred) 
    and cycle counts (difference of several hundred thousand). 
    For instance, for set-assoc, (no-write-allocate + write-through): 
                        lru           fifo
        Load misses:    6584          7180
        Store misses:   32667         33781
        Total cycles:   22858632      23095322
    The differences are not as significant as those seen when changing write policies. 


From these experiments and analysis, we see that the most effective cache configuration is 
a set-associative cache that uses write-back + write-allocate and lru eviction.

We performed a final set of experiments to compare effectiveness of different block sizes 
(using the current most-effective cache configuration).

    Halving block size (doubling # blocks): 
    ./csim 512 4 8 write-allocate write-back lru < gcc.trace
        Load hits: 313390
        Load misses: 4807
        Store hits: 179658
        Store misses: 17828
        Total cycles: 8938248
    Doubling block size (halving # blocks): 
    ./csim 128 4 32 write-allocate write-back lru < gcc.trace
        Load hits: 315689
        Load misses: 2508
        Store hits: 192637
        Store misses: 4849
        Total cycles: 11622726

    Summarizing:    
                        32          16            8
        Load misses:   2508         3399         4807
        Store misses:  4849         9236         17828
        Total cycles:  11622726     9966648      8938248

    These experiments showed that a smaller block size with greater number of sets resulted in fewer total cycles,
    with a tradeoff in increased store/load misses. Because our goal is to reduce cycle count to improve overall 
    performance, we conclude that a smaller block size gives a more effective cache.


Thus, we conclude that the cache configuration with the best overall effectiveness is 
a set-associative cache that uses write-back + write-allocate and lru eviction, with smaller (8 byte) block sizes for greater
number of sets.

Given the set cache size of 16,384 bytes, the most effective configuration is:
    512 4 8 write-allocate write-back lru

***** Additional Analysis *****
    If we don't keep the cache size constant, we found out that increasing the size of cache will increase cache efficiency.

    Halving the cache size (8,192):
    ./csim 256 4 8 write-allocate write-back lru < gcc.trace
        Load hits: 312586
        Load misses: 5611
        Store hits: 179374
        Store misses: 18112
        Total cycles: 9696360
    Oritinal cache size (16,384):
    ./csim 512 4 8 write-allocate write-back lru < gcc.trace
        Load hits: 313390
        Load misses: 4807
        Store hits: 179658
        Store misses: 17828
        Total cycles: 8938248
    Doubling cache size (32,768):
    ./csim 1024 4 8 write-allocate write-back lru < gcc.trace
        Load hits: 313804
        Load misses: 4393
        Store hits: 179909
        Store misses: 17577
        Total cycles: 8153313

    From the experiment above, we can see that increasing the cache size by increasing the number of sets decreases the
    total cycle as the number of misses decreases (there is more data in the cache).



