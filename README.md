Static Standard Template Library (SSTL)
-------------------------

**Summary**

The SSTL is a partial reimplementation of the C++ STL components.

The main design goal is to strictly avoid the use of dynamic memory allocation.

The SSTL exploits/supports modern C++ features.

**Motivation**

Although very handy, dynamic memory allocation is for many developers just an unaffordable luxury. In fact, the use of dynamic memory allocation might imply drawbacks such as:
- indeterministic behaviour
- synchronization costs in a multithreaded environmnent
- heap fragmentation
- additional memory requirements

There are applications in which such additional costs cannot be accepted. An example is the embedded world, where systems are often constrained to run under very thight memory constraints. Other examples are the fields of finance, gaming, etc. where the introduced indeterminism/latencies might just be too high.

**Features** 

Statically-allocated versions of:
- Most of the STL containers
- std::function

**Notes**

This project is a fork of the Embedded Template Library (http://www.etlcpp.com). The SSTL provides only a subset of the ETL components. However, the SSTL exploits/supports modern C++ features, whereas the ETL is strictly developed with C++03 in order to remain compatible with the "slow" vendors.

The fork just happened recently. So the SSTL doesn't exploit/support modern C++ features yet. I also haven't added std::function yet (the implementation is ready though). I haven't fully reorganized the structure of the repository, etc. However, such goodies should be coming soon.

