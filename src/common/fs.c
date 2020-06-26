#ifndef FC_LIB_FS
#define FC_LIB_FS

// Returns -1 on error, 0 otherwise
int load_read_file(char *file_name, char **out_ptr, size_t *out_len) {
    int f;
    if ((f = open(file_name, O_RDONLY)) == -1) {
        fprintf(stderr, "failed to load file %s with %s\n", file_name, strerror(errno));
        return -1;
    }
    struct stat f_stats;
    if (fstat(f, &f_stats) == -1) {
        fprintf(stderr, "failed to stat file %s with %s\n", file_name, strerror(errno));
        return -1;
    }
    char *data;
    if ((data = mmap(NULL, f_stats.st_size, PROT_READ, MAP_PRIVATE, f, 0)) == MAP_FAILED) {
        fprintf(stderr, "failed to mmap file %s with %s\n", file_name, strerror(errno));
        return -1;
    }
    *out_ptr = data;
    *out_len = f_stats.st_size;
    return 0;
}

// Returns -1 on error, 0 otherwise
int discard_read_file(char *ptr, size_t len) {
    if (munmap() == -1) {
        fprintf(stderr, "failed to munmap with %s\n", file_name, strerror(errno));
        return -1;
    }
    return 0;
}

#endif
