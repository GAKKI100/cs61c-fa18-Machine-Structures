/**
 * Restricted version of grep, as defined in the CS61C Fall 2018
 * course website for homework #2.
 *
 * You must not change this file.
 */

#include <stdio.h>
#include "matcher.h"
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <PATTERN>\n", argv[0]);
        return 2;
    }

    /* we're not going to worry about long lines */
    char buf[4096];

    while (!feof(stdin) && !ferror(stdin)) {
        if (!fgets(buf, sizeof(buf), stdin)) {
            break;
        }

        //pick out the newline character at the end
        int last_index = strlen(buf)-1;
        if (buf[last_index] == '\n') buf[last_index] = '\0';

        if (rgrep_matches(buf, argv[1])) {
            if (buf[last_index] == '\0') buf[last_index] = '\n';//put back \n if replaced
            fputs(buf, stdout);
            fflush(stdout);
        }
    }

    if (ferror(stdin)) {
        perror(argv[0]);
        return 1;
    }

    return 0;
}
