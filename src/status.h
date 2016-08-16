
#ifndef LAMINATE_STATUS_H
#define LAMINATE_STATUS_H

#include <string>

namespace laminate {

class Status {
  private:
  bool ok_;
  std::string *message_;

  public:

  Status();
  Status(std::string& m);

  bool Ok() const;

  ~Status();

  std::string Message() const;
};

Status Ok();
Status Fail(std::string& m);

}  // namespace laminate

#endif  // LAMINATE_STATUS_H
