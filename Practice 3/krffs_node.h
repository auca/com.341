#ifndef KRFFS_NODE_H
#define KRFFS_NODE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

struct krffs_file_system;

/*
    The maximum size of a file name buffer in a metadata node.
 */
#define KRFFS_FILE_NAME_BUFFER_SIZE 256

/*
    Types of metadata nodes which represent the file system.

        * KRFFS_Free_Node: a free, unused node
        * KRFFS_Reserved_Node: a reserved node which represents a file
 */
enum krffs_node_type {
    KRFFS_Root_Node,
    KRFFS_Free_Node,
    KRFFS_Reserved_Node,
};

/*
    Time specification for metadatа node fields
 */
#pragma pack(push,1)
struct krffs_node_time {
    int64_t seconds;
    int64_t nanoseconds;
};
#pragma pack(pop)

/*
    Gets the current time.
 */
struct krffs_node_time krffs_get_current_node_time();

/*
    Gets the node time as a number of seconds.
 */
time_t krffs_get_time_from_node_time(
           const struct krffs_node_time *node_time
       );

/*
    Gets the node time as a number of seconds and nanoseconds.
 */
struct timespec krffs_get_timespec_from_node_time(
           const struct krffs_node_time *node_time
       );

/*
    The metadata node used to describe blocks of the KRFFS file system.
 */
#pragma pack(push,1)
struct krffs_node {
    uint16_t magic;
    uint8_t type;
    uint64_t id;
    uint8_t name[KRFFS_FILE_NAME_BUFFER_SIZE];
    uint16_t mode;
    uint32_t uid;
    uint32_t gid;
    struct krffs_node_time access_time;
    struct krffs_node_time modification_time;
    struct krffs_node_time attribute_change_time;
    uint64_t size;
    uint64_t previous_node_size;
    uint64_t data_size;
};
#pragma pack(pop)

/*
    Initializes a free node.
 */
struct krffs_node *krffs_initialize_free_node(
                       struct krffs_node *node,
                       uint64_t size,
                       uint64_t previous_node_size
                   );

/*
    Initializes a root node.
 */
struct krffs_node *krffs_initialize_root_node(
                       struct krffs_node *node,
                       uint64_t size,
                       uint64_t previous_node_size
                   );

/*
    Sets the name of the node and updates the hash for the `id` field from the
    name.
 */
void krffs_set_reserved_node_name(
         struct krffs_node *node,
         const char *name
     );

/*
    Wraps around pointers to memory positions used by the `file_system`.
 */
struct krffs_node *krffs_wrap_node_position(
                       struct krffs_file_system *file_system,
                       struct krffs_node *node
                   );

/*
    Get the previous metadata node.

    Doesn't check node signatures.
 */
struct krffs_node *krffs_get_previous_node(
                       struct krffs_file_system *file_system,
                       struct krffs_node *node
                   );

/*
    Gets the next metadata node.

    Doesn't check node signatures.
 */
struct krffs_node *krffs_get_next_node(
                       struct krffs_file_system *file_system,
                       struct krffs_node *node
                   );

/*
    Gets the node relative position in the file system.
 */
uint64_t krffs_get_node_relative_position(
             struct krffs_file_system *file_system,
             struct krffs_node *node
         );

/*
    Checks that node is in range of addresses of a specified file system.
 */
bool krffs_is_node_in_file_system(
         struct krffs_file_system *file_system,
         struct krffs_node *node
     );

/*
    Searches the file system for a first node with a specified name. Uses a hash
    formed from the `name` to compare with the `id` field of each node. Returns
    the found node, or a `NULL` value otherwise.
 */
struct krffs_node *krffs_find_reserved_node_by_name(
                       struct krffs_file_system *file_system,
                       const char *name
                   );

/*
    Searches the file system for a first node with a specified name. Uses a hash
    formed from the `name` to compare with the `id` field of each node. Returns
    the found node, or a `NULL` value otherwise. Includes the root node in the
    search.
 */
struct krffs_node *krffs_find_reserved_or_root_node_by_name(
                       struct krffs_file_system *file_system,
                       const char *name
                   );

/*
    Count the number of free nodes.
 */
size_t krffs_count_free_nodes(
           struct krffs_file_system *file_system
       );

/*
    Calculate the total free space available in all free nodes.
 */
uint64_t krffs_calculate_total_free_space(
             struct krffs_file_system *file_system
         );

/*
    Count the number of reserved nodes.
 */
size_t krffs_count_reserved_nodes(
           struct krffs_file_system *file_system
       );

/*
    Calculate the total used space allocated by all reserved nodes.
 */
uint64_t krffs_calculate_total_reserved_space(
             struct krffs_file_system *file_system
         );

#endif
