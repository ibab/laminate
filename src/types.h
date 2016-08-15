#ifndef LAMINATE_TYPES_H
#define LAMINATE_TYPES_H

#include <H5Cpp.h>

// Static members are specialized in types.cc
template <typename T>
struct AsHdfType {
  static const H5::PredType value;
};

#endif  // LAMINATE_TYPES_H
