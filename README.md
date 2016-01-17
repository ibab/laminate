# protobuf-laminate

An efficiently packed data format for protocol buffer messages, written in C++.

The [Dremel paper](https://static.googleusercontent.com/media/research.google.com/en//pubs/archive/36632.pdf)
describes a columnar data format for storing nested records efficently.
The columnar nature of the data format allows for higher compression ratios and
more efficient queries compared to a row-based format (where one record is stored after the other).

There are several libraries out there, like [Apache Parquet](https://github.com/apache/parquet-mr),
which make use of ideas from the Dremel paper.
But there doesn't seem to be a library for storing protobuf messages using C++ at the moment.

The plan is to implement a custom Dremel-like data format with a simple API that interfaces nicely with protobuf.
The individual data columns will be compressed using the excellent [Blosc](http://www.blosc.org/) library.
The implementation of the Blosc interface can be found in [include/blosc_stream.h](include/blosc_stream.h).

A simple row-based format that demonstrates how the API could look has already been implemented in [include/row_store.h](include/row_store.h).
You can find an example on how to use it in the [examples](examples/) directory.
