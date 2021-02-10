# Flock based approach

## Environment
* There are 3 readers reading data from the file
* There is a single writer that writes data in chunks to the file

## Locking strategy
* The readers acquire lock with flock using LOCK_SH (shared lock) and hence multiple threads can read data simultaneously.
* The writer acquires a lock with flock using LOCK_EX (exclusive lock) and hence no other thread can access the file during this time.
* The sleep added in writer is significantly higher to infer if the exclusive lock is actually working.

## Results
From the below results, we can see that writers are not blocked and they have exclusive access to the file. Readers on the other hand have shared access when there is no writer lock active. 
``` shell
[Reader: 1]This is line number : 5
[Reader: 2]This is line number : 5
 Write started
 Write done
[Reader: 1]This is line number : 6
[Reader: 2]This is line number : 6
[Reader: 3]This is line number : 5
..
..
[Reader: 1]This is line number : 10
[Reader: 3]This is line number : 10
 Write started
 Write done
[Reader: 3]This is line number : 11
[Reader: 2]This is line number : 11
..
..
[Reader: 2]This is line number : 16
[Reader: 1]This is line number : 16
 Write started
 Write done
[Reader: 3]This is line number : 16
[Reader: 1]This is line number : 17
```