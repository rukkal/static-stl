Static Standard Template Library (SSTL)
-------------------------
Build status [![Build Status](https://travis-ci.org/rukkal/StaticSTL.svg)](https://travis-ci.org/rukkal/sstl)

**What is SSTL?**

The SSTL is a partial reimplementation of the C++ Standard Template Library (STL) that strictly avoids the use of dynamic memory allocation.

**Motivation**

Although very handy, dynamic memory allocation is for many developers an unaffordable luxury. In fact, the use of dynamic memory allocation might imply drawbacks such as:
- indeterministic behavior (in terms of latencies)
- synchronization costs in a multithreaded environmnent
- heap fragmentation
- additional memory requirements
- an additional level of indirection (worse locality of reference)

There are applications in which such additional costs cannot be accepted. An example is the embedded world, where systems are often constrained to run with very limited memory resources. Other examples are the fields of finance, gaming, etc. where the introduced indeterminism/latencies might just be too high.

**Features** 

- Statically-allocated reimplementations of STL abstractions (containers, std::function, std::shared_ptr, etc.).
- Leverages modern C++ features.

**How was SSTL born?**

I currently make a living from programming an embedded system with quite tight memory constraints. In that system I can't have a heap, since it would at least require some extra memory and I couldn't affort it. As a consequence, many of the much-loved STL components can't be used, since they often rely on dynamic memory allocations. To make up for this lack of abstractions I started to reimplement some STL components. For instance, I reimplemented std::vector, where an extra template parameter could be used to specify the fixed-capacity of the container. Such new vector could then be used as follows:
```c++
constexpr size_t MAX_ELEMENTS = 10;
auto v = sstl::vector<int, MAX_ELEMENTS>{};
```
All the SSTL components, unlike the STL counterparts, rely on an internal storage.

It was fun and relatively quick to reimplement std::vector, however I realized that reimplementing other abstractions such as std::unordered_map would have been more involved. So I look about for some existing work and I came across the Embedded Template Library (ETL) of John Wellbelove. The ETL provides fixed-capacity reimplementations of many STL containers as well as other generic components. Exactly what I needed one might think! Not quite... unfortunately the ETL has some characteristics that don't really fit my needs:
- Doesn't leverage modern C++ features and will keep doing so in order not to break compatibility with the slow vendors of C++ compilers for embedded systems.
- Performs runtime checks.
- Uses virtual functions.

In my case the runtime checks and use of virtual functions are especially undesired. In fact I often have to implement CPU intensive algorithms and additional runtime overheads would blow up my latencies and CPU usage. To fix these issues I decided to develop my own static STL library and so the SSTL was born! I started to develop the SSTL from a fork of the ETL, thus the SSTL was straight ready to be used at my company (thank you John Wellbelove for making this possible). However, I plan on either reimplement from scratch or drastically refactor most of the components that where originally in the ETL.

