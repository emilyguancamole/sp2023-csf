Esther Kwon and Emily Guan
kkwon16, eguan2

We worked together on all parts of this assignment :D

Eventually your report about how you implemented thread synchronization in the server should go here.

Please include where your critical sections are, how you determined them, 
and why you chose the synchronization primitives for each section. 
You should also explain how your critical sections ensure that the 
synchronization requirements are met without introducing synchronization 
hazards (e.g. race conditions and deadlocks).


Whenever we enqueued a mess

critical sections are sections of code where resources or variables are shared
by multiple threads or processes. In order to prevent synchronization hazards, these 
critical sections must be protected by synchronization mechanisms, i.e. locks and semaphores.

In our implementation of the chatroom, our critical sections include
when the message queue is manipulated, e.g. messages are enqueued, dequeued, or the queue is cleared. 
The message queue is a shared buffer, so when a sender/receiver is using it, it must have exclusive
access to the message queue to prevent data races.
Other critical sections include when members are added and removed from the room, as well as when the 
rooms stored in the map of rooms are changed (i.e. rooms are found or created). 
These sections are protected by locking and unlocking the mutex before and after the manipulation,
done by using pthread_mutex_lock and pthread_mutex_unlock or by using Guards.