#ifndef KRFFS_UTILITIES_H
#define KRFFS_UTILITIES_H

#include <stddef.h>
#include <stdint.h>

/*
    Calculates Bernstein's hash for a string.

    http://www.cse.yorku.ca/~oz/hash.html
 */
uint64_t krffs_calculate_djb_hash(uint8_t *string);

/*
    Returns a string `buffer` with a scaled representation of `size` in bytes in
    megabytes, gigabytes... or in bytes.
 */
char *krffs_scale_size(uint64_t size, char *buffer, size_t buffer_size);

#endif
