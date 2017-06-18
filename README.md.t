# libField

`libField` is a C++ library with a simple, clean interface for storing,
accessing, and manipulating fields (i.e. data with coordinates).
Its primary objective is to make writing programs that work with field data (such as
finite-difference based physics simulations) quick and simple.

Consider writing a 3-dimensional finite-difference heat solver. `libField` makes it simple
to create an array for the temperature and setup a coordinate system.

```
\shell{cat doc/examples/usage/demo-libField.cpp}
```
