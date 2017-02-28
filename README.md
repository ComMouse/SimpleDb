# SimpleDb
Simple Key-Value Database with persistent storage.

## Author
ComMouse

## Intro

A simple key-value database system with persistent storage. It stores hash table as index files and holds efficient caching management which supports memory size limitation. A linked list based freelist increases utilization rate of disk files. Data of arbitary size are supported. Another B-tree based index structure is reserved for future work.

Inspired by MySQL InnoDB & Redis.

## License

MIT License