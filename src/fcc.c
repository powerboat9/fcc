#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdint.h>

#include "vec.c"
#include "fs.c"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "no file provided\n");
        return -1;
    }
    struct memory_buffer loaded_file;
    if (load_read_file(argv[1], &loaded_file) == -1) {
        return -1;
    }
    printf("%.*s", loaded_file.len, loaded_file.data);
    return 0;
}
