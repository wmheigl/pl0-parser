#include <stdlib.h>
#include <string.h>
#include "options.h"

void print_usage(const char* program_name) {
    fprintf(stderr, "Usage: %s [options] input_file\n", program_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -d, --debug        Print AST\n");
    fprintf(stderr, "  -s, --symbols      Print symbol table\n");
    fprintf(stderr, "  -v, --verbose      Detailed output\n");
    fprintf(stderr, "  -o <file>          Write output to file\n");
    fprintf(stderr, "  --no-types         Skip type checking\n");
    fprintf(stderr, "  --no-semantics     Skip semantic analysis\n");
    fprintf(stderr, "  -h, --help         Print this help message\n");
}

Options parse_options(int argc, char** argv) {
    Options opts = {
        .print_ast = false,
        .print_symbols = false,
        .verbose = false,
        .skip_type_check = false,
        .skip_semantics = false,
        .input_file = NULL,
        .output = stdout
    };

    int i;
    for (i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            if (opts.input_file == NULL) {
                opts.input_file = argv[i];
            } else {
                fprintf(stderr, "Error: Multiple input files specified\n");
                print_usage(argv[0]);
                exit(1);
            }
            continue;
        }

        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            exit(0);
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            opts.print_ast = true;
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--symbols") == 0) {
            opts.print_symbols = true;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            opts.verbose = true;
        } else if (strcmp(argv[i], "--no-types") == 0) {
            opts.skip_type_check = true;
        } else if (strcmp(argv[i], "--no-semantics") == 0) {
            opts.skip_semantics = true;
        } else if (strcmp(argv[i], "-o") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "Error: -o requires a filename\n");
                print_usage(argv[0]);
                exit(1);
            }
            opts.output = fopen(argv[i], "w");
            if (!opts.output) {
                perror("Error opening output file");
                exit(1);
            }
        } else {
            fprintf(stderr, "Error: Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            exit(1);
        }
    }

    if (opts.input_file == NULL) {
        fprintf(stderr, "Error: No input file specified\n");
        print_usage(argv[0]);
        exit(1);
    }

    return opts;
}

