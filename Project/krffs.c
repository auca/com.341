#include "krffs.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef WINDOWS
#include <io.h>
#else
#include <unistd.h>
#endif

/*
    The size of buffer to store a path to a file with the file system.
 */
#define KRFFS_PATH_BUFFER_LENGTH 1024

/*
    krffs

    A simple FUSE File System in a File based on a K&R `next-fit` memory
    allocator.

    Usage:
        krffs <file> <mount point> <FUSE options>
 */
int main(int argc, char **argv)
{
    int exit_status =
        EXIT_SUCCESS;

    int file_descriptor = -1;
    struct krffs_file_system file_system = {
        .node = NULL
    };

    /*
        Check parameters from the command line.

        Our parameters FUSE parameters
        <file>         <mount point> <FUSE options>

        Without the <file> parameter, pass everything untouched directly
        to FUSE.
     */
    if (argc < 3) {
        exit_status =
            fuse_main(
                argc,
                argv,
                &krffs_operations,
                NULL
            );

        goto cleanup;
    } else {
        char *path =
            argv[1];
        char *mount_point =
            argv[2];

        if (*path == '-' || *mount_point == '-') {
            exit_status =
                fuse_main(
                    argc,
                    argv,
                    &krffs_operations,
                    NULL
                );

            goto cleanup;
        }
    }

    /*
        Duplicate the path to a file with the file system as we are going to
        remove it before passing arguments to FUSE.
     */
    char path[KRFFS_PATH_BUFFER_LENGTH + 1];
    path[KRFFS_PATH_BUFFER_LENGTH] = '\0';
    strncpy(
        path,
        argv[1],
        KRFFS_PATH_BUFFER_LENGTH
    );

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
        Check that the file is big enough to contain a file system.
     */
    if (file_info.st_size < sizeof(*file_system.node) * 2) {
        fprintf(
            stderr,
            "The file at '%s' is not big enough to contain a file system.\n",
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
        Check that we have a KRFFS file system by checking the signature at the
        beginning of the file.
     */
    if (file_system.node->magic != KRFFS_File_System_Magic) {
        fprintf(
            stderr,
            "The file system was not found at '%s'. The file system was not "
            "created or the file is corrupted.\n"
            "\n"
            "To create a file system use the `mkfs.krffs` program.\n",
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
        Replace our argument with an option to force FUSE to start in a
        single-threaded mode.
     */
    argv[1] = "-s";

    /*
        Set the global file system pointer.
     */
    File_System_Store =
        file_system;
    KRFFS_File_System =
        &File_System_Store;

    /*
        Call FUSE. Pass its parameters (the mount point with options) and all
        function pointers to our file system handlers.
     */
    exit_status =
        fuse_main(
            argc,
            argv,
            &krffs_operations,
            NULL
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
