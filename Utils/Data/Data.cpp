#include "Data.h"


int compressData(const char *input, size_t inputSize, char **output, size_t *outputSize) {
    z_stream stream;
    memset(&stream, 0, sizeof(stream));

    // Initialize compression stream with best compression level
    if (deflateInit(&stream, Z_BEST_COMPRESSION) != Z_OK) {
        return -1;
    }

    // Allocate memory for output buffer
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

    // Store compressed data size
    *outputSize = stream.total_out;

    // Clean up compression stream
    deflateEnd(&stream);

    return 0;
}


int decompressData(const char *input, size_t inputSize, char **output, size_t *outputSize) {
    z_stream stream;
    memset(&stream, 0, sizeof(stream));

    // Initialize decompression stream
    if (inflateInit(&stream) != Z_OK) {
        return -1;
    }

    // Allocate memory for output buffer
    *output = (char *)malloc(inputSize * 2);
    stream.avail_in = inputSize;
    stream.next_in = (unsigned char *)input;
    stream.avail_out = inputSize * 2;
    stream.next_out = (unsigned char *)(*output);

    // Decompress input data
    int result = inflate(&stream, Z_FINISH);

    if (result != Z_STREAM_END) {
        inflateEnd(&stream);
        free(*output);
        return -1;
    }

    // Store decompressed data size
    *outputSize = stream.total_out;

    // Clean up decompression stream
    inflateEnd(&stream);

    return 0;
}
