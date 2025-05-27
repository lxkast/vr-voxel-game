#pragma once

#include <stdio.h>
#include <stdint.h>

/*
    Example use:


    futil_file_t *fp = futil_open("code.b", FUTIL_FILE_READ);

    int size = futil_fileSize(fp);
    uint32_t *buf = (uint32_t *)malloc(size * sizeof(uint32_t));

    futil_readBinary(fp, buf, size);

    futil_close(fp);
*/

typedef FILE futil_file_t;

typedef enum {
    FUTIL_FILE_READ,
    FUTIL_FILE_WRITE
} futil_fileOpenMode_t;


/*
    This function opens a file, returning a pointer to it.
*/
futil_file_t *futil_open(const char *fileName, futil_fileOpenMode_t mode);


/*
    Closes a file
*/
void futil_close(futil_file_t *fp);

/*
    Returns the number of 32 bit chunks of the file.
*/
unsigned int futil_fileSize(futil_file_t *fp);

/*
    Reads bytes to a buffer of uint32_t.
    Reads maximum n 32 bit chunks.
    Returns the number of 32 bit chunks read.
*/
unsigned int futil_readBinary(futil_file_t *fp, uint32_t *buf, int n);

/*
    Writes bytes from a buffer of uint32_t to the file.
    Tries to write n 32 bit chunks.
    Returns the number of 32 bit chunks written.
*/
unsigned int futil_writeBinary(futil_file_t *fp, const uint32_t *buf, int n);
