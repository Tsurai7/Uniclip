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

    if (deflateInit(&stream, Z_BEST_COMPRESSION) != Z_OK) {
        exit(EXIT_FAILURE);
    }

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

    *outputSize = stream.total_out;

    deflateEnd(&stream);
}


void decompressData(const char *input, size_t inputSize, char **output, size_t *outputSize) {
    z_stream stream;
    memset(&stream, 0, sizeof(stream));

    if (inflateInit(&stream) != Z_OK) {
        exit(EXIT_FAILURE);
    }

    *output = (char *)malloc(inputSize * 2);
    stream.avail_in = inputSize;
    stream.next_in = (unsigned char *)input;
    stream.avail_out = inputSize * 2;
    stream.next_out = (unsigned char *)(*output);

    int result = inflate(&stream, Z_FINISH);

    if (result != Z_STREAM_END) {
        inflateEnd(&stream);
        exit(EXIT_FAILURE);
    }

    *outputSize = stream.total_out;

    inflateEnd(&stream);
}
