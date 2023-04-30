Esther Kwon and Emily Guan
kkwon16, eguan2

We worked together on all parts of this assignment :D

Critical sections are sections of code where resources or variables are shared
by multiple threads or processes. In order to prevent synchronization hazards, these 
critical sections must be protected by synchronization mechanisms.
In our case, we used semaphores, which are synchronization objects that keep count of the thread
in order to limit the number of threads that are accessing the code at a given time. They also
helps prevent dead lock, as they ensure resources are not held indefinitely by one thread.

Thread synchronization happens mainly when we modify the room and the message queue.
In room.cpp and server.cpp, the users in the room and the map containing all rooms are modified through the 
functions add_member(), remove_member(), and find_or_create_room(). Critical sections in these 
functions were protected to make sure that changes made to set of User pointers in the Room class and
changes made to Room pointers in the map remain atomic, and thus accurately track all the receivers 
in the room and the rooms created. Protection was done by using a Guard object, which is a wrapper for a mutex,
to lock each function. Since Guard objects guarantee the mutex will be unlocked when the Guard object goes
out of scope, this prevents deadlocks.

In message_queue.cpp and room.cpp, critical sections were located in enqueue(), dequque() and
broadcast_to_all(). To make sure that changes and interactions with the messages within the
queue were atomic, we used Guard objects and semaphores. 
For example, a Guard object was created in enqueue() to make push_back() atomic, protecting
the message queue from being manipulated by multiple threads at once. Then, sem_post()
was called on the semaphore to signal the receiver that there was a message being sent.
In dequeue(), the semaphore checked for any changes in the queue every second. If there was a change,
the message would be dequed from the queue. Before popping from the message queue, we ensured that
the mutex was locked to protect this shared resource, then unlocked it after manipulation.