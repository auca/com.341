#ifndef KRFFS_FILE_SYSTEM_H
#define KRFFS_FILE_SYSTEM_H

#include <stdint.h>
#include <sys/types.h>

struct krffs_node;

/*
    The magic value at the beginning of each metadata node. Magic values can be
    used to identify and check file system consistency.

    Should be equal to ASCII codes of characters K and R.
 */
static const uint16_t KRFFS_File_System_Magic = 0x4B52;

/*
    The file system's root node name.
 */
static const char KRFFS_File_System_Root_Node_Name[] = "";

/*
    The file system's root node mode.
 */
static const uint16_t KRFFS_File_System_Root_Node_Mode = 0755;

/*
    General file system data. Nodes point to memory pages mapped to a file.
 */
struct krffs_file_system
{
    struct krffs_node *node;
    off_t size;
};

/*
    Initializes the file system by creating a root metadata node and a free
    space node after it.
 */
struct krffs_file_system *krffs_initialize_file_system(
                              struct krffs_file_system *file_system
                          );

#endif
