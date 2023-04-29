Esther Kwon and Emily Guan
kkwon16, eguan2

We worked together on all parts of this assignment :D

Eventually your report about how you implemented thread synchronization in the server should go here.

Please include where your critical sections are, how you determined them, 
and why you chose the synchronization primitives for each section. 
You should also explain how your critical sections ensure that the 
synchronization requirements are met without introducing synchronization 
hazards (e.g. race conditions and deadlocks).


critical sections are sections of code where resources or variables are shared
by multiple threads or processes. In order to prevent synchronization hazards, these 
critical sections must be protected by synchronization mechanisms.
In our case, we used semaphore, which is a synchronization object that keeps count of the thread
in order to limit the number of threads that is accessing the code at a given time. This also
helps prevent dead lock.

Thread synchronization happens mainly when we modify the room and the message queue.

In room.cpp and server.cpp, the room and the map containing room is modified through functions:
add_member(), remove_member(), and find_or_create_room(). Critical sections was added to these 
functions to make sure that changes made to set of User pointers in the Room class and
changes made to Room pointers in the map remain atomic. This helps keep track of all the receivers 
in the room and the rooms created.
This was done by using the Guard object, which is a mutex

In message_queue.cpp and room.cpp, critical sections were added to enqueue(), dequque() and
broadcast_to_all(). This makes sure that changes and interactions with the messages within the
queue are atomic. This was done by using the guard object and semaphores. 
For example, the Guard object was made in enqueue to make push_back() atomic. Then, sem_post()
was called on the semaphore to signal the receiver that there is a message being sent.
In dequeue(), the semaphore would check for any changes in the queue every second. If it is about
success, the message would be dequed from the queue. In doing this, a guard object was also used
when dequeue is valid to prevent unnecessary locking in the remaining function.