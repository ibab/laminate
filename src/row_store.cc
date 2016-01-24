/*
 * The functions `writeDelimitedTo` and `readDelimitedTo` have
 * been adapted from this StackOverflow answer by Kenton Varda:
 * http://stackoverflow.com/a/22927149
 *
 */

#include "row_store.h"

#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include "blosc_stream.h"

using namespace laminate;

using google::protobuf::Message;
using google::protobuf::io::ZeroCopyOutputStream;
using google::protobuf::io::ZeroCopyInputStream;
using google::protobuf::io::FileOutputStream;
using google::protobuf::io::FileInputStream;

bool writeDelimitedTo(const google::protobuf::MessageLite &message,
                      google::protobuf::io::ZeroCopyOutputStream *rawOutput) {
  // We create a new coded stream for each message. Don't worry, this is fast.
  google::protobuf::io::CodedOutputStream output(rawOutput);

  // Write the size.
  const int size = message.ByteSize();
  output.WriteVarint32(size);

  uint8_t *buffer = output.GetDirectBufferForNBytesAndAdvance(size);
  if (buffer != NULL) {
    // Optimization:  The message fits in one buffer, so use the faster
    // direct-to-array serialization path.
    message.SerializeWithCachedSizesToArray(buffer);
  } else {
    // Slightly-slower path when the message is multiple buffers.
    message.SerializeWithCachedSizes(&output);
    if (output.HadError()) return false;
  }

  return true;
}

bool readDelimitedFrom(google::protobuf::io::ZeroCopyInputStream *rawInput,
                       google::protobuf::MessageLite *message) {
  // We create a new coded stream for each message. Don't worry, this is fast,
  // and it makes sure the 64MB total size limit is imposed per-message rather
  // than on the whole stream.
  google::protobuf::io::CodedInputStream input(rawInput);

  // Read the size.
  uint32_t size;
  if (!input.ReadVarint32(&size)) return false;

  google::protobuf::io::CodedInputStream::Limit limit = input.PushLimit(size);
  // Parse the message.
  if (!message->MergeFromCodedStream(&input)) return false;
  if (!input.ConsumedEntireMessage()) return false;
  input.PopLimit(limit);

  return true;
}

RowStore::RowStore(std::string filename, std::string mode) {
  int omode = 0;
  if (mode == "w") {
    omode = O_CREAT | O_WRONLY | O_TRUNC;
  } else if (mode == "r") {
    omode = O_RDONLY;
  }
  fd_ = open(filename.c_str(), omode, 0600);
  is_open_ = fd_ != -1;
  output_ = nullptr;
  input_ = nullptr;
  if (mode == "w") {
    file_out_ = new FileOutputStream(fd_);
    auto options = BloscOutputStream::DefaultOptions();
    // Bit shuffling would only permute our record contents
    // and would make inhomogeneous data harder to compress,
    // so we disable it here.
    options.use_shuffling = false;
    // We pick zlib here, which means we will need longer
    // to compress the data, but this should give use a better
    // compression ratio.
    options.compressor = "zlib";
    options.compression_level = 7;
    output_ = new BloscOutputStream(file_out_, options);
  } else if (mode == "r") {
    file_in_ = new FileInputStream(fd_);
    input_ = new BloscInputStream(file_in_);
  }
}

RowStore::~RowStore() {
  if (input_ == nullptr) {
    delete output_;
    delete file_out_;
  }
  if (output_ == nullptr) {
    delete input_;
    delete file_in_;
  }
  if (IsOpen()) {
    Close();
  }
}

void RowStore::Put(const google::protobuf::Message &message) {
  if (!output_) {
    std::cerr << "RowStore not opened in write mode" << std::endl;
    exit(1);
  }
  writeDelimitedTo(message, output_);
}

bool RowStore::ReadNext(google::protobuf::Message *message) {
  if (!input_) {
    std::cerr << "RowStore not opened in read mode" << std::endl;
    exit(1);
  }
  message->Clear();
  return MergeNext(message);
}

bool RowStore::MergeNext(google::protobuf::Message *message) {
  if (!input_) {
    std::cerr << "RowStore not opened in read mode" << std::endl;
    exit(1);
  }
  return readDelimitedFrom(input_, message);
}

bool RowStore::IsOpen() { return is_open_; }

void RowStore::Close() {
  close(fd_);
  is_open_ = false;
}
