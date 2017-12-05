#ifndef KRFFS_ALLOCATOR_H
#define KRFFS_ALLOCATOR_H

#include <stdint.h>

struct krffs_file_system;
struct krffs_node;

/*
    K&R `first-fit` allocator's malloc function to find and reserve a block of a
    specified `size`.
 */
struct krffs_node *krffs_allocate_reserved_node(
           struct krffs_file_system *file_system,
           uint64_t size,
           struct krffs_node *original_node
       );

/*
    K&R `first-fit` allocator's realloc function to resize a used block.
 */
struct krffs_node *krffs_resize_reserved_node(
           struct krffs_file_system *file_system,
           struct krffs_node *node,
           uint64_t size
       );

/*
    K&R `first-fit` allocator's free function to remove a used block.
 */
void krffs_remove_reserved_node(
         struct krffs_file_system *file_system,
         struct krffs_node *node
     );

#endif
