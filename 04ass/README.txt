CONTRIBUTIONS

Esther Kwon kkwon16@jhu.edu
Emily Guan

Worked on all the parts together by meeting up in person.

REPORT

TODO: add your report according to the instructions in the
"Experiments and analysis" section of the assignment description.


Threshold: 2097152
real    0m0.567s
user    0m0.530s
sys     0m0.019s

Threshold: 1048576
real    0m0.519s
user    0m0.737s
sys     0m0.044s

Threshold: 524288
real    0m0.435s
user    0m0.680s
sys     0m0.075s

Threshold: 262144
real    0m0.375s
user    0m0.763s
sys     0m0.091s

Threshold: 131072
real    0m0.317s
user    0m0.726s
sys     0m0.129s

Threshold: 65536
real    0m0.328s
user    0m0.732s
sys     0m0.163s

Threshold: 32768
real    0m0.341s
user    0m0.751s
sys     0m0.225s

Threshold: 16384
real    0m0.214s
user    0m0.676s
sys     0m0.249s

Questions:
We implemented the merge sort as a parallel recursive function that splits the process into two child by forking.
When forked, the child processes are spawned and runs simultaneously, resulting in reduce of time.

For our experiment, we are looking at the real time since it best indicates the amount of time elapsed between when the program
is started and exited. 
From the result, we can see that there is a general decreasing trend in real time for threshold between 2097152 and 131072.
This makes sense as decreasing the threshold would increase the number of child processes spawned, resulting in more parallel
processing for merge sort - performed more efficiently in less time. 
However, after this threshold, the real time increases towards the end. This can be explained by the 
overhead costs incurred when the kernel switches from one process to another. With more child processes, the kernel has to
perform this switch more frequently. Additionally, the system is bound by the cpu core. This means that once the number of
child processes spawned exceeds the number that can be handled by the cpu core, the kernel is spending more time to switch
between the processes than actually computing the program.

The small drecrease of real time for the last threshold may have been caused by noise and other users in the ugrad system,
leading to a result that goes against the increasing trend.