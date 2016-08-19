#include "hdf_stream.h"

namespace laminate {

#define DEFINE_HDFSTREAM(T) \
  template <>                  \
  class HDFOutputStream<T>;    \
  template <>                  \
  class HDFInputStream<T>;

DEFINE_HDFSTREAM(int);
DEFINE_HDFSTREAM(long);
DEFINE_HDFSTREAM(float);
DEFINE_HDFSTREAM(double);

#undef DEFINE_HDFSTREAM

}
