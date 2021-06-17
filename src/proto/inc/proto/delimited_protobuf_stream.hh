#ifndef CUTE_PROTO_DELIMITED_PROTOBUF_STREAM_HH_
#define CUTE_PROTO_DELIMITED_PROTOBUF_STREAM_HH_

#include <iostream>
#include <thread>

const int DELIMITED_MESSAGE_MAX_SIZE = 4096;

template <class T>
class DelimitedProtobufStream {
public:
    DelimitedProtobufStream(T& message)
        : message(message)
    {
        buffer = nullptr;
        valid = false;
        size = 0;
    }

    ~DelimitedProtobufStream()
    {
        if (buffer) {
            delete[] buffer;
        }
    }

    operator bool() const { return valid; }

    template <class U>
    friend std::istream& operator>>(std::istream& is, DelimitedProtobufStream<U>& stream);

    template <class U>
    friend std::ostream& operator<<(std::ostream& os, DelimitedProtobufStream<U>& stream);

private:
    uint8_t* buffer;
    size_t size;

    T& message;
    bool valid;
};

template <class T>
std::istream& operator>>(std::istream& is, DelimitedProtobufStream<T>& stream)
{
    uint64_t size = 0;
    is.read((char*)&size, sizeof(size));

    if (is.good() && size < DELIMITED_MESSAGE_MAX_SIZE) {
        if (size > stream.size) {
            delete[] stream.buffer;
            stream.buffer = new uint8_t[size];
        }

        is.read((char*)stream.buffer, size);
        stream.valid = stream.message.ParseFromArray(stream.buffer, (int)size);
    } else {
        stream.valid = false;
    }

    return is;
}

template <class T>
std::ostream& operator<<(std::ostream& os, DelimitedProtobufStream<T>& stream)
{
    uint64_t size = stream.message.ByteSizeLong();
    os.write((char*)&size, sizeof(size));
    stream.message.SerializeToOstream(&os);
    return os;
}

#endif