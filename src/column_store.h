#ifndef LAMINATE_COLUMN_STORE_H
#define LAMINATE_COLUMN_STORE_H

#include <vector>
#include <string>
#include <google/protobuf/message.h>

#include "column_writer.h"
#include "status.h"

namespace laminate {

class ColumnStore {
  public:
  ColumnStore(std::string path, std::string mode);
  Status Put(google::protobuf::Message &m);

  private:
  ColumnWriter writer_;
  std::string path_;
  bool initialized_;
};

}  // namespace laminate

#endif  // LAMINATE_COLUMN_STORE_H
