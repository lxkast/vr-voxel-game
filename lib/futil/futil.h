#pragma once

#include <stdio.h>
#include <stdint.h>

/*
    Example use:


    futil_file_t *fp = futil_open("code.b");

    int size = futil_fileSize(fp);
    uint32_t *buf = (uint32_t *)malloc(size);

    int read = futil_readBinary(fp, buf, size);
*/

typedef FILE futil_file_t;


/*
    This function opens a file, returning a pointer to it.
*/
futil_file_t *futil_open(const char *fileName);


/*
    Closes a file
*/
void futil_close(futil_file_t *fp);


/*
    Returns the size (bytes) of the file.
*/
int futil_fileSize(futil_file_t *fp);


/*
    Reads bytes to a buffer of uint32_t.
    Reads maximum n bytes.
    Returns the number of bytes read.
*/
int futil_readBinary(futil_file_t *fp, uint32_t *buf, int n);


/*
    Writes bytes from a buffer of uint32_t to the file.
    Tries to write n bytes.
    Returns the number of bytes written.
*/
int futil_writeBinary(futil_file_t *fp, uint32_t *buf, int n);