#include <iostream>
#include "blosc_stream.h"

using google::protobuf::io::ZeroCopyOutputStream;

BloscOutputStream::BloscOutputStream(ZeroCopyOutputStream* output, int typesize) 
   : buffer_size_(1000000),
     output_(output),
     bytes_written_(0),
     fresh_(true),
     typesize_(typesize) {
       buffer_ = operator new(buffer_size_);
}

BloscOutputStream::~BloscOutputStream() {
    // FIXME
    compressAndPush();
    operator delete(buffer_);
}

void BloscOutputStream::compressAndPush() {

    int blosc_max_buffer_size = buffer_size_ + BLOSC_MAX_OVERHEAD;
    void* blosc_buffer = operator new(blosc_max_buffer_size);

    int blosc_buffer_size = blosc_compress_ctx(
            5, // clevel
            1, // doshuffle
            typesize_, // typesize
            buffer_size_, // nbytes
            buffer_, // src
            blosc_buffer, // dest
            buffer_size_ + BLOSC_MAX_OVERHEAD, // destsize
            "blosclz", // compressor
            0, // blocksize
            4  // numinternalthreads
    );

    void* out_buffer;
    int out_size;

    int written = 0;

    while (written < blosc_buffer_size) {
        output_->Next(&out_buffer, &out_size);
        if (written + out_size > blosc_buffer_size) {
            // This is the last chunk
            memcpy(out_buffer, (char*)blosc_buffer + written, blosc_buffer_size - written);
            // Let's back up the bit we don't want to write
            output_->BackUp(out_size - (blosc_buffer_size - written));
            written = blosc_buffer_size;
        } else {
            // We're not finished yet
            memcpy(out_buffer, (char*)blosc_buffer + written, out_size);
            written += out_size;
        }
    }

    bytes_written_ += buffer_size_;

    operator delete(blosc_buffer);
}

bool BloscOutputStream::Next(void** data, int* size) {

    // Perform compression of data from last call to Next(),
    // unless this is the very first call
    if (!fresh_) {
        compressAndPush();
    } else {
        fresh_ = false;
    }

    *data = buffer_;
    *size = buffer_size_;

    return true;
}

void BloscOutputStream::BackUp(int count) {
    buffer_size_ -= count;
}

int64 BloscOutputStream::ByteCount() const {
    return bytes_written_;
}

