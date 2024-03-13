#include "Data.h"


int compressData(const char *input, size_t inputSize, char **output, size_t *outputSize) {
    z_stream stream;
    memset(&stream, 0, sizeof(stream));

    if (deflateInit(&stream, Z_BEST_COMPRESSION) != Z_OK) {
        return -1;
    }

    // Размер выходного буфера должен быть примерно равен размеру входного буфера
    *output = (char *)malloc(inputSize * 2);
    stream.avail_in = inputSize;
    stream.next_in = (unsigned char *)input;
    stream.avail_out = inputSize * 2;
    stream.next_out = (unsigned char *)(*output);

    int result = deflate(&stream, Z_FINISH);
    if (result != Z_STREAM_END) {
        deflateEnd(&stream);
        free(*output);
        return -1;
    }

    *outputSize = stream.total_out;

    deflateEnd(&stream);

    return 0;
}


int decompressData(const char *input, size_t inputSize, char **output, size_t *outputSize) {
    z_stream stream;
    memset(&stream, 0, sizeof(stream));

    if (inflateInit(&stream) != Z_OK) {
        return -1;
    }

    *output = (char *)malloc(inputSize * 2);
    stream.avail_in = inputSize;
    stream.next_in = (unsigned char *)input;
    stream.avail_out = inputSize * 2;
    stream.next_out = (unsigned char *)(*output);

    int result = inflate(&stream, Z_FINISH);

    if (result != Z_STREAM_END) {
        inflateEnd(&stream);
        free(*output);
        return -1;
    }

    *outputSize = stream.total_out;

    inflateEnd(&stream);

    return 0;
}