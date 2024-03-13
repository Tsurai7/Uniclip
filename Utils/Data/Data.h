#ifndef UNICLIP_DATA_H
#define UNICLIP_DATA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

int compressData(const char *input, size_t inputSize, char **output, size_t *outputSize);
int decompressData(const char *input, size_t inputSize, char **output, size_t *outputSize);

#endif //UNICLIP_DATA_H
