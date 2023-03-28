Esther Kwon, Emily Guan

To determine the cache configuration with the best overall effectiveness, we ran experiments with the gcc.trace
file for each possible cache configuration. That is, we tested direct-mapped, fully associative, and set associative
configurations. 

We tested write policies: (write-allocate + write-back), (write-allocate + write-through), and 
(no-write-allocate + write-through) for each configuration.

For each combination of configuration and the above parameters, we repeated the tests with lru and fifo 
(fifo was not done with direct-mapping, as eviction type does not matter).

The number of sets and blocks per set were changed to alter the cache configuration 
(i.e. direct-map has n sets of 1 block, fully associative cache has 1 set of n blocks).
However, the total cache capacity was kept at 2^10=1024. Each block consistently contained 16 bytes of memory.

***** EXPERIMENTS *****
The commands to simulate each experiment, along with results, are summarized below.

*** write-allocate + write-back ***
** lru **
Direct-mapped (1 block per set):
./csim 1024 1 16 write-allocate write-back lru < gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 312238
    Load misses: 5959
    Store hits: 187502
    Store misses: 9984
    Total cycles: 12626940
Fully associative (1 set):
./csim 1 1024 16 write-allocate write-back lru < gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 314973
    Load misses: 3224
    Store hits: 188300
    Store misses: 9186
    Total cycles: 9756473
Set-associative (256 sets, 4 blocks each):
./csim 256 4 16 write-allocate write-back lru < gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 314798
    Load misses: 3399
    Store hits: 188250
    Store misses: 9236
    Total cycles: 9966648
** fifo **
Fully associative:
./csim 1 1024 16 write-allocate write-back fifo < gcc.trace
    Total loads: 318197
    Total stores: 197486
    Load hits: 314311
    Load misses: 3886
    Store hits: 188117
    Store misses: 9369
    Total cycles: 10419228
Set-associative:
./csim 256 4 16 write-allocate write-back fifo < gcc.trace
    Total loads: 318197
    Total stores: 197486
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
    Load hits: 311160
    Load misses: 7037
    Store hits: 164196
    Store misses: 33290
    Total cycles: 23038756
Set-associative:
    Load hits: 311017
    Load misses: 7180
    Store hits: 163705
    Store misses: 33781
    Total cycles: 23095322


***** ANALYSIS *****

When comparing across cache configurations (direct mapping vs. fully asssociative vs. set associative),
the main difference is seen in total cycle count. 
    For (write-allocate + write-back) & lru, direct-mapping had about 2 million more total cycles than associative mapping. 
    For (write-allocate + write-through) & lru, direct mapping had over 1.3 million more total cycles than associative mapping.
    For (no-write-allocate + write-through) & lru, direct mapping had about 1 million more total cycles (the smallest difference
    among lru eviction).
    The other statistics were not significantly different. 

    This analysis is generally true for both lru and fifo eviction. However, we did notice that differences
    in total cycles for fifo eviction were smaller than those for lru eviction. The smallest of these 
    differences was seen in (no-write-allocate + write-through): direct mapping had about 800 thousand more cycles.


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

^^ DATA for lru, (write-allocate + write-back) vs. (write-allocate + write-through) vs. (no-write-allocate + write-through)
    fully assoc
        Load hits: 314973
        Load misses: 3224
        Store hits: 188300
        Store misses: 9186
        Total cycles: 9756473

        Load hits: 314973
        Load misses: 3224
        Store hits: 188300
        Store misses: 9186
        Total cycles: 25215873

        Load hits: 311723
        Load misses: 6474
        Store hits: 164778
        Store misses: 32708
        Total cycles: 22814701

    set assoc
        Load hits: 314798
        Load misses: 3399
        Store hits: 188250
        Store misses: 9236
        Total cycles: 9966648

        Load hits: 314798
        Load misses: 3399
        Store hits: 188250
        Store misses: 9236
        Total cycles: 25305648

        Load hits: 311613
        Load misses: 6584
        Store hits: 164819
        Store misses: 32667
        Total cycles: 22858632


We did further comparisons of results for lru vs. fifo eviction.
    Overall, lru eviction was slightly more efficient than fifo eviction, as it had lower miss rates (differences of several hundred) 
    and cycle counts (difference of several hundred thousand). 
    For instance, for set-assoc, (no-write-allocate + write-through): 
                        lru           fifo
        Load misses:    6584          7180
        Store misses:   32667         33781
        Total cycles:   22858632      23095322
    The differences are not as significant as those seen when changing write policies. 


From our experiments and analysis, we see that the cache configuration with the best overall effectiveness is 
a set-associative cache that uses write-back + write-allocate and lru eviction.

We performed a final set of experiments to compare effectiveness of different block sizes 
(using the current most-effective cache configuration).
                        32          16            8
        Load misses:   2071         3399         5611
        Store misses:  4663         9236         18112
        Total cycles:  10077749     9966648      9696360
    Decreasing block size increased number of misses but decreased number of cycles.
    Increasing block size decreased the number of load and store misses. However there was a tradeoff in increased 
    total cycle count. Therefore, we conclude that changing block size doesn't really matter???






  **!!!!!!! check below - the overhead analysis might be wrong !!!!!!!**



When considering cache overhead for each configuration, we considered the boolean storing 'dirty' values, 
as this information is only used with write-back. From this, write-back would require more overhead. However,
since our cache class contained the same information for all types of classes, there would not be a functional difference
for the cache simulator.

