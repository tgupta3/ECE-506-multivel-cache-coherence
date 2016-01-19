# ECE-506-multivel-cache-coherence
ECE-506 Multivel cache-coherence protocol simulator by Tushar Gupta

To Run
1. make
2. ./smp_cache L1_SIZE L2_SIZE l1_ASSOC L2_ASSOC BLOCKSIZE TRACEFILE

Implements write through,write no-allocate as the write policy and uses inclusion principle with MESI as the protocol in a multicore envrionment. Each processor has a private L1 and a L2 cache and all caches are connected to the main meory thorugh a shared bus. 
