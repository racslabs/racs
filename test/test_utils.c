
#include "test_utils.h"


void read_file(const char *path, racs_uint8 **out, size_t *size) {
    *out = NULL;
    *size = 0;

    FILE *file = fopen(path, "rb");
    if (!file) {
        return;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return;
    }

    long file_size = ftell(file);
    if (file_size < 0) {
        fclose(file);
        return;
    }

    rewind(file);

    if (file_size == 0) {
        fclose(file);
        return;
    }

    racs_uint8 *buf = malloc(file_size);
    if (!buf) {
        fclose(file);
        return;
    }

    size_t bytes_read = fread(buf, 1, file_size, file);
    if (bytes_read == 0) {
        free(buf);
        fclose(file);
        return;
    }

    *out = buf;
    *size = bytes_read;
    fclose(file);
}

void write_file(const char *path, const racs_uint8 *data, size_t size) {
    if (!path || (!data && size != 0)) {
        return;
    }

    FILE *file = fopen(path, "wb");
    if (!file) {
        return;
    }

    if (size != 0) {
        size_t bytes_written = fwrite(data, 1, size, file);
        if (bytes_written != size) {
            fclose(file);
            return;
        }
    }

    fclose(file);
}
