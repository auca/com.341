#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "krffs_file_system.h"
#include "krffs_node.h"
#include "krffs_platform.h"

#ifdef WINDOWS
#include <io.h>
#else
#include <unistd.h>
#endif

/*
    mkfs.krffs

    Creates a KRFFS FUSE file system in a file.

    Usage:
        mkfs.krffs -h
        mkfs.krffs <file> [-f]

    Options:
        -h    show help and exit
        -f    force file system creation
 */
int main(int argc, char **argv)
{
    int exit_status =
        EXIT_SUCCESS;

    int file_descriptor = -1;
    struct krffs_file_system file_system = {
        .node = NULL
    };

    bool has_force_option = false;
    bool has_help_option  = false;
    for (int i = 1; i < argc; ++i) {
        if (!has_force_option ||
                strncmp(argv[i], "-f", 2) == 0) {
            has_force_option = true;
        } else if (!has_help_option ||
                strncmp(argv[i], "-h", 2) == 0) {
            has_help_option = true;
        }
    }

    if (argc <= 1 || has_help_option) {
        fprintf(
            stderr,
            "Usage: %s <file> <options>\n",
            argc >= 1 ?
                argv[0] : "mkfs.krffs"
        );

        goto cleanup;
    } else if (argv[1][0] == '-') {
        fprintf(
            stderr,
            "The first parameter is invalid.\n"
            "\n"
            "Usage: %s <file> <options>\n",
            argc >= 1 ?
                argv[0] : "mkfs.krffs"
        );

        exit_status =
            EXIT_FAILURE;

        goto cleanup;
    }

    char *path =
        argv[1];

    /*
        Open the file with the file system.
     */
    if ((file_descriptor = PLATFORM_PREFIX(open(path, O_RDWR))) == -1) {
        fprintf(
            stderr,
            "Failed to open the file system file at '%s'.\n",
            path
        );

        exit_status =
            EXIT_FAILURE;

        goto cleanup;
    }

    /*
        Get file information.
     */
    struct stat file_info;
    if (fstat(file_descriptor, &file_info) == -1) {
        fprintf(
            stderr,
            "Failed to get file information for '%s'.\n",
            path
        );

        exit_status =
            EXIT_FAILURE;

        goto cleanup;
    }

    /*
        Check that we have a regular file (not a directory or a socket).
     */
    if (!S_ISREG(file_info.st_mode)) {
        fprintf(
            stderr,
            "The file system file at '%s' is not a regular file.\n",
            path
        );

        exit_status =
            EXIT_FAILURE;

        goto cleanup;
    }

    /*
        Check that the file is big enough to initialize a file system.
     */
    if (file_info.st_size < sizeof(*file_system.node) * 2) {
        fprintf(
            stderr,
            "The file at '%s' is not big enough to initialize a file system.\n",
            path
        );

        exit_status =
            EXIT_FAILURE;

        goto cleanup;
    }

    /*
        Save the size of the file.
     */
    file_system.size =
        file_info.st_size;

    /*
        Map the file system file into memory. Changes to memory at
        `file_system.node` after a successful call to `krffs_map_file` will be
        written directly to a file (right away or after calls to
        `krffs_unmap_file` or `krffs_sync_mapping`).

        The kernel uses its virtual memory system to implement the memory
        mapping.
     */
    if ((file_system.node =
             krffs_map_file(
                 file_descriptor,
                 0,
                 file_system.size
             )) == (void *) -1) {
        fprintf(
            stderr,
            "Failed to map the file system file at '%s' into memory.\n",
            path
        );

        exit_status =
            EXIT_FAILURE;

        goto cleanup;
    }

    /*
        It's possible to close the file after a call to `krffs_map_file`. Memory
        pages will still be mapped to the file.
     */
    if (PLATFORM_PREFIX(close(file_descriptor)) == -1) {
        fprintf(
            stderr,
            "Failed to close the file system file at '%s'.\n",
            path
        );

        exit_status =
            EXIT_FAILURE;

        goto cleanup;
    }
    file_descriptor = -1;

    /*
        Try to create a file system.
     */
    struct krffs_node *node = file_system.node;
    if (node->magic == KRFFS_File_System_Magic && !has_force_option) {
        fprintf(
            stderr,
            "Found a valid file system at '%s'. Will not proceed without the "
            "Force ('-f') option.\n",
            path
        );

        exit_status =
            EXIT_FAILURE;

        goto cleanup;
    }

    krffs_initialize_file_system(
        &file_system
    );

cleanup:
    if (file_descriptor != -1) {
        if (PLATFORM_PREFIX(close(file_descriptor)) == -1) {
            fprintf(
                stderr,
                "Failed to close the file system file.\n"
            );
        }
    }

    if (file_system.node != NULL) {
        if (krffs_unmap_file(
                file_system.node,
                file_system.size
            ) == -1) {
            fprintf(
                stderr,
                "Failed to unmap the file system file.\n"
            );
        }
    }

    return exit_status;
}
