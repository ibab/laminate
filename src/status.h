
#ifndef LAMINATE_STATUS_H
#define LAMINATE_STATUS_H

#include <string>

namespace laminate {

class Status {

  public:
  Status() : ok_(true), message_("") {}
  Status(const std::string& m) : ok_(false), message_(m) {}

  static Status OK() { return Status(); }
  static Status Error(const std::string& m) { return Status(m); }

  bool Ok() const {
    return ok_;
  }

  explicit operator bool() const {
    return Ok();
  }

  std::string Message() const {
    if (ok_) {
      return "OK";
    }
    return message_;
  }

  void Update(const Status& status) {
    if (!status) {
      ok_ = false;
      message_ = status.Message();
    }
  }

  private:
  bool ok_;
  std::string message_;
};

std::ostream& operator<<(std::ostream& os, const Status& x);

}  // namespace laminate

#endif  // LAMINATE_STATUS_H
