# Data Structures and Algorithms: A C++ Approach

This is the code for the book *Data Structures and Algorithms: A C++ Approach*
by Aldin Kovačević. Every listing printed in the book is copied from a file in
this repository, and every table of measured times comes from one of the
`experiments.cpp` programs here, so you can run them on your own machine and
compare.

There is one folder per chapter. When the book names a file such as
`code/ch03/LinkedList.tpp`, that is `ch03/LinkedList.tpp` here.

| Folder | Chapter |
|---|---|
| `ch01` | Introduction |
| `ch02` | A C++ Primer |
| `ch03` | Linked Lists |
| `ch04` | Stacks and Queues |
| `ch05` | Analysis of Algorithms |
| `ch06` | Search Algorithms |
| `ch07` | Elementary Sorts |
| `ch08` | Merge Sort |
| `ch09` | Quick Sort |
| `ch10` | Radix Sort and Counting Sort |

## Running the code

You need a C++17 compiler: `g++`, `clang++`, or the one that comes with Visual
Studio or Xcode. Nothing else is required, except CMake for Chapter 2, which
uses it as its example.

Each chapter has a `tests.cpp` that checks every listing in that chapter. The
first lines of every program say how to compile it. For example:

```sh
cd ch03
c++ -std=c++17 -Wall -Wextra -O2 -o tests tests.cpp && ./tests
```

Chapter 2 has a second source file, so it is compiled with both:

```sh
cd ch02
c++ -std=c++17 -Wall -Wextra -O2 -o tests tests.cpp Student.cpp && ./tests
```

To run the tests of every chapter at once:

```sh
for d in ch*/; do
  (cd "$d" && c++ -std=c++17 -Wall -Wextra -O2 -o tests tests.cpp \
     $( [ -f Student.cpp ] && echo Student.cpp ) && ./tests)
done
```

The `experiments.cpp` programs produce the measured tables in the book. Your
times will differ from the printed ones, because they depend on the machine,
but the way they grow with the input size will not.

## A note on the exercises

Some exercises at the end of a chapter ask you to write code that is already
here. `ch03/DoublyLinkedList.tpp`, for example, is one solution to the doubly
linked list exercise in Chapter 3, and `ch03/doubly_tests.cpp` checks it. Write
your own version first, then compare.

This repository is published from the book's sources. Corrections go into the
book first and then appear here.
