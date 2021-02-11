![Build](https://github.com/saai63/FileLock_comparison/workflows/Linux_Build/badge.svg)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# FileLock_comparison
The intention here is to have a mechanism where-in multiple processes and threads can read parallely from a file, but only one thread can perform a write.
When a thread is writing data, all other threads should be blocked.

# flock
In Linux, this can be pretty easily done with a call to flock ( At least I hope so )
But this approach is not a cross platform solution.

# std::shared_lock
C++-14 provides a construct known as shared_lock. 
This provides for a truly cross platform solution.
