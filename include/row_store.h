#ifndef LAMINATE_ROW_STORE_H
#define LAMINATE_ROW_STORE_H

/*
 * Implementation of a simple row-based store for protocol
 * buffer messages.
 * Messages are stored one after the other.
 * Each message is prefixed with a varint that contains the
 * message length in bytes.
 *
 * The size of each message must not exceed 64 MB.
 *
 * This is designed for cases where a row-based store is
 * desired instead of the more efficiently packed column store.
 */

#include <google/protobuf/message.h>
#include <google/protobuf/io/zero_copy_stream.h>

namespace laminate {
class RowStore {
  public:
  /*
   * Either opens up or creates a new row-based store
   * for protocol buffer messages.
   *
   * Set mode = "w" to create a new file to write to.
   * Set mode = "r" to read from an existing file created
   * with RowStore.
   */
  RowStore(std::string filename, std::string mode);
  /*
   * Frees up the underlying file descriptor.
   */
  ~RowStore();
  /*
   * Appends the given protocol buffer message to the
   * RowStore.
   * Remember to Clear() your message if you are reusing
   * it in a loop!
   */
  void Put(const google::protobuf::Message &message);
  /*
   * Reads the next protocol buffer message contained
   * in the RowStore into the given message.
   * The given message is automatically cleared before
   * reading when passing it to this function.
   */
  void ReadNext(google::protobuf::Message &message);
  /*
   * Reads the next protocol buffer message contained
   * in the RowStore into the given message.
   * The given message is not cleared, which means
   * that the fields will be merged into it.
   */
  void MergeNext(google::protobuf::Message &message);
  /*
   * Checks if the store is currently open.
   */
  bool IsOpen();
  /*
   * Closes the underlying file descriptor.
   */
  void Close();

  private:
  int fd_;
  bool is_open_;
  google::protobuf::io::ZeroCopyOutputStream *file_out_;
  google::protobuf::io::ZeroCopyOutputStream *output_;
  google::protobuf::io::ZeroCopyInputStream *file_in_;
  google::protobuf::io::ZeroCopyInputStream *input_;
};
}

#endif
