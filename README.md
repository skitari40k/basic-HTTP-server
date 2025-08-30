# basic-HTTP-server
Basic HTTP server using C.

A simple HTTP server in C
This project is a simple multithreaded HTTP server written in C that processes GET requests and serves static files.

# Features
- Multithreaded query processing using pthreads
- URL decoding support
- Automatic detection of MIME types
- Protection against path traversal attacks
- Support for large files (up to 100 MB)

# Build and launch
make

./server

The server will be running on port 8080. You can use a browser or curl for testing.:

curl http://localhost:8080/test.html

To make sure it displays .html correctly, you can go to localhost:8080/test.html

# Problems and solutions

1. Processing URL-encoded paths

Problem: Clients can send URL-encoded characters in the file path

Solution: The urldecode() function is implemented to convert %-encoded sequences.

2. Defining MIME types

Problem: Correct definition of Content-Type for different file types

Solution: The get_mime() function has been created, which maps file extensions to MIME types.

3. Path safety

Problem: The ability to bypass directories via ../ in the path

Solution: Added a check for availability.. in the file path

4. Multithreading

Problem: Efficient handling of multiple connections

Solution: Implemented thread pulling using pthreads

5. Large files

Problem: Processing large files

Solution: Increased the buffer size to 100 MB (BUF_SIZE)

# Possible improvements
- File Caching - Adding an LRU cache for frequently requested files
- Thread Pooling - Using a thread pool instead of creating a new one for each request
- Non-blocking I/O - Implementation of asynchronous request processing using epoll/kqueue
- HTTPS support - Adding encryption via SSL/TLS
- Configuration file - The ability to configure the port, root directory, and other parameters
- Logging - Adding a system for logging requests and errors
- HTTP/1.1 support - Implementation of keep-alive connections and chunked transfer encoding
