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
  - std::function (OK)
  - std::shared_ptr (TODO)
  - std::vector (OK)
  - std::list (REFACTORING REQUIRED)
  - std::forward_list (REFACTORING REQUIRED)
  - std::deque (REFACTORING REQUIRED)
  - std::set (REFACTORING REQUIRED)
  - std::multiset (REFACTORING REQUIRED)
  - std::map (REFACTORING REQUIRED)
  - std::multimap (REFACTORING REQUIRED)
  - std::unordered_set (TODO)
  - std::unordered_multiset (TODO)
  - std::unordered_map (TODO)
  - std::unordered_multimap (TODO)
  - std::stack (OK)
  - std::queue (OK)
  - std::priority_queue (OK)
  - bitmap allocation policy (REFACTORING REQUIRED)
  - free-list allocation policy (TODO)
- No RTTI used.
- No exceptions required (however the components marked with "OK" are exception safe).
- No virtual functions used (except for sstl::function's type erasure, of course).
- No runtime check overheads (only customizable assertions in debug build).
- C++11 compatible.
- Header-only library.
- Tested with clang 3.7, gcc 5 and MSVC 1800 (Visual Studio 2013).

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

I currently make a living from programming CPU-intensive algorithms on an embedded system with quite tight memory constraints. In that system I can't have a heap, since it would at least require some extra memory that I can't afford. As a consequence, many of the much-loved STL components can't be used, since they heavily rely on dynamic memory allocations. To make up for this lack of abstractions I started to reimplement some STL components.

It was fun and relatively quick to reimplement std::vector, however I realized that reimplementing other abstractions such as std::unordered_map would have been more involved. So I looked about for some existing work and I came across the Embedded Template Library (ETL) of John Wellbelove. The ETL provides fixed-capacity reimplementations of many STL containers as well as other generic components. Exactly what I needed one might think! Not quite... unfortunately the ETL has some characteristics that don't really fit my needs:
- Performs runtime checks.
- Part of the library uses virtual functions.
- Doesn't leverage move semantics and perfect forwarding (doesn't leverage modern C++ features in general and will keep doing so in order not to break compatibility with the slow vendors of C++ compilers for embedded systems).
- Memory footprint of components is not always as small as it might be.
- No customizable allocation policies (pools used to allocate nodes of linked lists and BSTs).

In my case the runtime checks, the virtuals and the lack of move semantics and perfect forwarding were especially undesired. In fact having such additional runtime overheads in my algorithms' hotspots would have blown up my CPU usage. To fix these issues I decided to develop my own static STL library and so the SSTL was born! I started to develop the SSTL from a duplication of the ETL, thus many STL reimplementations of John were straight ready to be used at my company (at least in the non-performance-critical code). Thank you John for making this possible! However, I plan on either reimplement from scratch or drastically refactor many of the components that where originally in the ETL.
