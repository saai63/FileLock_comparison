# FileLock_comparison
The intention here is to have a mechanism where-in multiple processes and threads can read parallely from a file, but only one thread can perform a write.
When a thread is writing data, all other threads should be blocked.

# flock
In Linux this can be pretty easily done with a call flock ( At least I hope so )

# std::shared_lock
