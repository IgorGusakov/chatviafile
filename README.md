# test_Samsung
TASK<
Two processes operate with one file without delaying (no sleep or timer). 
Maximum performance is the goal.>

### Progress 
- performance (DONE)
- documentation (DONE)
- unit-test (IN PROGRESS...)
- bugs (DONE)

### Description
This program works with a file that must be located
inside the directory of the running process. 
You can use 2 arguments to run ``` ./testfile -rw -crc ```

- Program operation option: *!!!One of these options is required to run!!!*
  * ``` -r ``` *Runs a read-only program on a file*
  * ``` -w ``` *Runs a write-only program on a file*
  * ``` -rw ``` *Runs a read-write program on a file, Splits the program into two threads, one reads, the other writes*


- File hashing option:
  * ``` -crc ``` *An additional option that, if specified, will enable the calculation of the hashing of the file* 

If you want to change the location of a file, its name or maximum file size, 
you need to open *main.cpp* and change
```   
  //name file
  std::string name_test_file = "Testing.txt";
  //max size file
  uint64_t max_size_file = 5'000'000;
```
#### Flaws
 - Can't specify file path via CLI
 - File open handler or exception that will throw an open error (if it cannot be created)
 - When reading a file, the thread loads the processor due to the endless comparison of the file size, a callback is needed that will silently wait for the file to change
 - Interprocess Synchronization (sockets, pipes, shared memory ...) for writing and reading

### Building
 - For build, you need cmake(3.13 or high), LLVM(GCC/CLANG with support c++17).
 - ``` sudo apt install cmake g++``` Install requirement
 - ``` mkdir build && cd build ``` Create folder build
 - ``` cmake .. ``` run cmake build
 - ``` make -j8 ``` run build
 - ``` ctest --extra-verbose ``` run unit tests
 - ``` ./testfile -rw -crc``` run program(flag run see above)

### Detailed code documentation
- For detail read code generate doxygen we can run ``` ./doc/html/index.html ```