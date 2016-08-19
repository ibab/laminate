
#include "status.h"

namespace laminate {

std::ostream& operator<<(std::ostream& os, const Status& x) {
  os << x.Message();
  return os;
}

}  // namespace laminate
