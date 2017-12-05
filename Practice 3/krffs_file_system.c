#include "krffs_file_system.h"

#include "krffs_node.h"

#include <assert.h>
#include <stddef.h>

struct krffs_file_system *krffs_initialize_file_system(
                              struct krffs_file_system *file_system
                          )
{
    assert(file_system && file_system->node);

    struct krffs_node *root_node =
        file_system->node;

    uint64_t root_node_size =
        sizeof(*root_node);
    uint64_t free_node_size =
        file_system->size -
            root_node_size;

    krffs_initialize_root_node(
        root_node,
        root_node_size,
        free_node_size
    );

    struct krffs_node *free_node =
        krffs_get_next_node(
            file_system,
            root_node
        );

    krffs_initialize_free_node(
        free_node,
        free_node_size,
        root_node_size
    );

    return file_system;
}
