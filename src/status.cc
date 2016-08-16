#include "status.h"

namespace laminate {

Status::Status() : ok_(true) {}
Status::Status(std::string& m) : ok_(false), message_(new std::string(m)) {}

bool Status::Ok() const {
  return ok_;
}

Status::~Status() {
  if (message_) {
    delete message_;
  }
}

std::string Status::Message() const {
  return *message_;
}

Status Ok() {
  return Status();
}

Status Fail(std::string& m) {
  return Status(m);
}

}  // namespace laminate
