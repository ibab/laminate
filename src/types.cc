
#include "src/types.h"

#define DEFINE_HDFTYPE(N, H) \
  template<> const H5::PredType AsHdfType<N>::value = H5::PredType::H

DEFINE_HDFTYPE(int, NATIVE_INT);
DEFINE_HDFTYPE(long, NATIVE_LONG);
DEFINE_HDFTYPE(float, NATIVE_FLOAT);
DEFINE_HDFTYPE(double, NATIVE_DOUBLE);

#undef DEFINE_HDFTYPE
