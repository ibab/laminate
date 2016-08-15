
#include "src/types.h"

#define DEFINE_TYPE(N, H) \
  template<> const H5::PredType AsHdfType<N>::value = H5::PredType::H

DEFINE_TYPE(int, NATIVE_INT);
DEFINE_TYPE(float, NATIVE_FLOAT);

#undef DEFINE_TYPE
