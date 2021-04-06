# std::atomic based approach

## Environment
* There are a large number(50) of readers reading data from the file
* There can be multiple writers that write data in chunks to the file

## Locking strategy
* We use a spin lock designed around std::atomic to emulate the locks.
* Multiple readers should be able to read the data simultaneously and hence when new readers are added we just increment the count of readers.
* Writers should have exclusive access and hence should be allowed only when the number of readers are 0 and there are no other writers.
* The above strategy is inefficient when there are many readers. At any point of time there might be a reader and hence reader count may never come to 0, this is avoided by signalling the presence of writer and making all readers wait till the writer is done writing.

## Results
From the below results, we can see that writers are not blocked and they have exclusive access to the file. Readers on the other hand have shared access when there is no writer lock active. 
``` shell
[Reader : 48] This is line number : 0
[Reader : [Reader : [Reader : 4847] This is line number : 11]
This is line number : 4] This is line number : 2

[Reader : [Reader : [Reader : 48471] ] This is line number : 5] This is line number : 2This is line number : 3


 Write started
 Write done
[Reader : 16] This is line number : 1
[Reader : [Reader : [Reader : 1516] 3] ] This is line number : 1This is line number : 1This is line number : 2


[Reader : [Reader : 1615] This is line number : 3] This is line number : 2

[Reader : [Reader : 15] This is line number : 3
[Reader : 16] This is line number : 4
7] This is line number : 1
[Reader : [Reader : [Reader : 71615] ] This is line number : 5] This is line number : 4
This is line number : 2

 Write started
 Write done
[Reader : [Reader : [Reader : 2534] ] 27This is line number : 1] This is line number : 1This is line number : 1
```