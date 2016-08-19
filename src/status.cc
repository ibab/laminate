
#include "status.h"

using namespace laminate;

std::ostream& operator<<(std::ostream& os, const Status& x) {
  os << x.Message();
  return os;
}
