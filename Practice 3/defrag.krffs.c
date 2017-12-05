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
    Name of the program to check the file system.
 */
#define KRFFS_FILE_SYSTEM_CHECK_UTILITY_NAME "./fsck.krffs "

/*
    The size of buffer to store a command.
 */
#define KRFFS_COMMAND_BUFFER_LENGTH 1024

/*
    defrag.krffs

    Defragments a KRFFS FUSE file system in a file.

    Usage:
        defrag.krffs -h
        defrag.krffs <file>

    Options:
        -h    show help and exit
 */
int main(int argc, char **argv)
{
    int exit_status =
        EXIT_SUCCESS;

    int file_descriptor = -1;
    struct krffs_file_system file_system = {
        .node = NULL
    };

    bool has_help_option  = false;
    for (int i = 1; i < argc; ++i) {
        if (strncmp(argv[i], "-h", 2) == 0) {
            has_help_option = true;
            break;
        }
    }

    if (argc <= 1 || has_help_option) {
        fprintf(
            stderr,
            "Usage: %s <file>\n",
            argc >= 1 ?
                argv[0] : "defrag.krffs"
        );

        goto cleanup;
    } else if (argv[1][0] == '-') {
        fprintf(
            stderr,
            "The first parameter is invalid.\n"
            "\n"
            "Usage: %s <file>\n",
            argc >= 1 ?
                argv[0] : "defrag.krffs"
        );

        exit_status =
            EXIT_FAILURE;

        goto cleanup;
    }

    char *path =
        argv[1];

    /*
        Ensure that the file system is valid by calling the file
        system check utility.

        You can remove this check, if you don't have a working
        `fsck.krffs`.
     */

    char command_buffer[KRFFS_COMMAND_BUFFER_LENGTH + 1];
    command_buffer[KRFFS_COMMAND_BUFFER_LENGTH] = '\0';

    size_t command_length =
        strlen(KRFFS_FILE_SYSTEM_CHECK_UTILITY_NAME);

    strncpy(
        command_buffer,
        KRFFS_FILE_SYSTEM_CHECK_UTILITY_NAME,
        KRFFS_COMMAND_BUFFER_LENGTH
    );
    strncat(
        command_buffer,
        path,
        KRFFS_COMMAND_BUFFER_LENGTH -
            command_length
    );

    int fsck_result =
        system(command_buffer);

    if (fsck_result == -1 &&
            fsck_result == 127) {
        command_length =
            strlen("./"KRFFS_FILE_SYSTEM_CHECK_UTILITY_NAME);

        strncpy(
            command_buffer,
            "./"KRFFS_FILE_SYSTEM_CHECK_UTILITY_NAME,
            KRFFS_COMMAND_BUFFER_LENGTH
        );
        strncat(
            command_buffer,
            path,
            KRFFS_COMMAND_BUFFER_LENGTH -
                command_length
        );
        fsck_result =
            system(command_buffer);
    }

    if (fsck_result != 0) {
        fprintf(
            stderr,
            "Failed check the file system file at '%s'.\n",
            path
        );

        exit_status =
            EXIT_FAILURE;

        goto cleanup;
    }

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
        Check that we have a KRFFS file system by checking the signature at the
        beginning of the file.
     */
    if (file_system.node->magic != KRFFS_File_System_Magic) {
        fprintf(
            stderr,
            "The file system was not found at '%s'. The file system was not "
            "created or the file is corrupted.\n",
            path
        );

        exit_status =
            EXIT_FAILURE;

        goto cleanup;
    }

    /*
        Defragment a file system by moving free space between used (reserved)
        blocks toward the end.

        Example
            node: [type, size in bytes]
                   free
                   used

                [free, 2]{  }[used, 5]{     }[free, 3]{   }[used, 2]{  }

            Move the first free block toward the end while relocating reserved
            (used) blocks. Combine consecutive free blocks to the right.

            Find the first free block.

                [free, 2]{  }[used, 5]{     }[free, 3]{   }[used, 2]{  }
                ^
                |
                -

            Move it one step toward the end.

                [free, 2]{  }[used, 5]{     }[free, 3]{   }[used, 2]{  }
                |                            ^
                |____________________________|

            Combine consecutive free blocks.

                [used, 5]{     }[free, 2]{  }[free, 3]{   }[used, 2]{  }
                                |            ^
                                |____________|
                                       +

                [used, 5]{     }[free, 5 (2+3)]{     }[used, 2]{  }

            Continue moving toward the end.

                [used, 5]{     }[free, 5]{     }[used, 2]{  }
                                |                            ^
                                |____________________________|

                [used, 5]{     }[used, 2]{  }[free, 5]{     }
     */

    // TODO

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
