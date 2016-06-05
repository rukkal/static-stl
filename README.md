Static Standard Template Library (SSTL) [![Build Status](https://travis-ci.org/rukkal/static-stl.svg?branch=master)](https://travis-ci.org/rukkal/static-stl) [![Build Status](https://travis-ci.org/rukkal/static-stl.svg?branch=develop)](https://travis-ci.org/rukkal/static-stl)
-------------------------

**What is SSTL?**

The SSTL is a partial reimplementation of the C++ Standard Template Library (STL) that strictly avoids the use of dynamic memory allocation.

**Motivation**

Although very handy, dynamic memory allocation is for many developers an unaffordable luxury. In fact, the use of dynamic memory allocation implies drawbacks such as:
- indeterministic behavior (in terms of latencies)
- synchronization costs in a multithreaded environmnent
- additional memory requirements (heap bookkeeping + fragmentation)
- worse locality of reference

There are applications in which such additional costs cannot be accepted. An example is the embedded world, where systems are often constrained to run with very limited memory resources. Other examples are the fields of finance, gaming, etc. where the introduced indeterminism/latencies might just be too high.

**Features** 

- Static reimplementations of STL abstractions:
  - std::function
  - std::vector
  - std::deque
  - std::stack
  - std::queue
  - std::priority_queue
  - bitmap allocator
  - free-list allocator
- No RTTI used.
- No exceptions required (however all the components are exception safe).
- No virtual functions used (except for sstl::function's type erasure, of course).
- No runtime check overheads (only customizable assertions).
- C++11 compatible.
- Header-only library.
- Tested with clang 3.7, gcc 5 and MSVC 1800 (Visual Studio 2013).

**TODO list**
- std::shared_ptr
- std::unordered_set
- std::unordered_map

**Example**

For example, the SSTL provides a reimplementation of std::vector that can be used as follows:
```c++
constexpr size_t MAX_ELEMENTS = 10;
sstl::vector<int, MAX_ELEMENTS> vec{};
sstl::vector<int>& ref = vec;
ref.push_back(0);
```
The elements are stored into an internal buffer whose capacity is specified by the extra template argument. Note that this template argument is required only for the creation of the container. The created instance can then be used and passed around as a reference to a capacity-agnostic type (e.g. sstl::vector&lt;int&gt;).

**How was SSTL born?**

I used to make a living from programming CPU-intensive algorithms on an embedded system with quite tight memory constraints. In that system I couldn't have a heap, since it would have required some extra memory that I couldn't afford. As a consequence I couldn't use many of the much-loved STL components, since they heavily rely on dynamic memory allocations. To make up for this lack of abstractions I started to reimplement some STL components.
