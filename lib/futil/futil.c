#include <stdint.h>
#include <stdio.h>
#include "futil.h"

futil_file_t *futil_open(const char *fileName, const futil_fileOpenMode_t mode) {
    FILE *fp = NULL;
    switch (mode) {
        case FUTIL_FILE_READ:
            fp = fopen(fileName, "rb");
            break;
        case FUTIL_FILE_WRITE:
            fp = fopen(fileName, "wb");
            break;
    }
    return fp;
}

void futil_close(futil_file_t *fp) {
    fclose(fp);
}

unsigned int futil_fileSize(futil_file_t *fp) {
    fseek(fp, 0L, SEEK_END);
    const unsigned int n = ftell(fp) / sizeof(uint32_t);
    rewind(fp);
    return n;
}

unsigned int futil_readBinary(futil_file_t *fp, uint32_t *buf, const unsigned int n) {
    const unsigned int nRead = fread(buf, sizeof(uint32_t), n, fp);
    rewind(fp);
    return nRead;
}

unsigned int futil_writeBinary(futil_file_t *fp, const uint32_t *buf, const int n) {
    return fwrite(buf, sizeof(uint32_t), n, fp);
}
