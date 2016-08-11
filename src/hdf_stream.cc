
#include "hdf_stream.h"

using google::protobuf::io::ZeroCopyOutputStream;
using google::protobuf::io::ZeroCopyInputStream;

namespace laminate {

HDFOutputStream::HDFOutputStream(ZeroCopyOutputStream *output,
    const std::string& fname, const std::string& dataset, const Options& options)
  : output_(output) {
};

HDFInputStream::HDFInputStream(ZeroCopyInputStream *input)
  : input_(input) {
};

}
