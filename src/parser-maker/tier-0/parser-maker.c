#include <stdlib.h>
#include <stdio.h>

#include "../common/fs.c"

/*

Accepts BNF and converts into parsing code

Code 

*/

typedef struct {
    unsigned char cnt;
} TokenDef;

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "[parser-builder] usage: %s (input file) (output header file) (output c file)\n", argv[0]);
        return -1;
    }
    char *input_file_name = argv[1];
    char *output_header_file_name = argv[2];
    char *output_c_file_name = argv[3];
    char *in_data;
    size_t in_len;
    if (load_read_file(input_file_name, &in_data, &in_len) == -1) {
        fprintf(stderr, "[parser-builder] failed to read file");
        return -1;
    }
}
