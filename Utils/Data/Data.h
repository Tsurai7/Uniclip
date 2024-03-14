#ifndef UNICLIP_DATA_H
#define UNICLIP_DATA_H

#include <stdlib.h>


void compressData(const char *input, size_t inputSize, char **output, size_t *outputSize);
void decompressData(const char *input, size_t inputSize, char **output, size_t *outputSize);

#endif
