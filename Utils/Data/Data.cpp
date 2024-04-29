#include "Data.h"

#include <string.h>
#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <iostream>

void compressData(const char *input, size_t inputSize, char **output, size_t *outputSize) {
    z_stream stream;
    memset(&stream, 0, sizeof(stream));

    // Initialize compression stream with best compression level
    if (deflateInit(&stream, Z_BEST_COMPRESSION) != Z_OK) {
        exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
    }

    // Store compressed data size
    *outputSize = stream.total_out;

    // Clean up compression stream
    deflateEnd(&stream);
}


void decompressData(const char *input, size_t inputSize, char **output, size_t *outputSize) {
    z_stream stream;
    memset(&stream, 0, sizeof(stream));

    // Initialize decompression stream
    if (inflateInit(&stream) != Z_OK) {
        exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
    }

    // Store decompressed data size
    *outputSize = stream.total_out;

    // Clean up decompression stream
    inflateEnd(&stream);
}
