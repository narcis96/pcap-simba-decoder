Single Threaded: Processing time: 54.3077 seconds
Decoding Concurrent: Processing time: 35.6295 seconds
BatchWrite: Processing time: 31.65 seconds
Dedicated thread for writes: 29.2148 seconds (not added in the final code)
Reduce Number of I/O operation on read: 22.0346 seconds
4 threads only: 13.5334 seconds
Use std::queue instead of std::vector for |futures| : 13.1579 seconds