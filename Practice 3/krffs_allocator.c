#include "krffs_allocator.h"

#include "krffs_file_system.h"
#include "krffs_node.h"
#include "krffs_platform.h"
#include "krffs_utilities.h"

#include <stdbool.h>
#include <string.h>

struct krffs_node *krffs_allocate_reserved_node(
                       struct krffs_file_system *file_system,
                       uint64_t size,
                       struct krffs_node *original_node
                   )
{
    struct krffs_node *result = NULL;

    if (!file_system || !file_system->node) {
        return result;
    }

    uint64_t size_with_header =
        sizeof(struct krffs_node) + size;

    struct krffs_node *node =
        file_system->node;

    do {
        if (node->type == KRFFS_Free_Node && node->size >= size_with_header) {
            if (node->size > size_with_header) {
                /* found a larger block, splitting it in half */
                uint64_t total_size =
                    node->size;
                uint64_t second_half_size =
                    total_size - size_with_header;

                /* the second part should be big enough to contain a header */
                if (second_half_size >= sizeof(*node)) {
                    /* mark the first half as a used block */
                    node->size =
                        size_with_header;

                    /* move to the second half */
                    struct krffs_node *next_node =
                        krffs_get_next_node(
                            file_system,
                            node
                        );

                    /* mark the second half as a free block */
                    krffs_initialize_free_node(
                        next_node,
                        second_half_size,
                        size_with_header
                    );

                    /* move to the next block */
                    next_node =
                        krffs_get_next_node(
                            file_system,
                            next_node
                        );

                    /* update the size of the previous block */
                    next_node->previous_node_size =
                        second_half_size;

                    krffs_sync_mapping(
                        next_node,
                        sizeof(*next_node)
                    );
                }
            }

            /* initialize a new reserved node */
            node->type =
                KRFFS_Reserved_Node;
            node->data_size =
                size;

            if (original_node) {
                /* copy data from the original node */
                strncpy(
                    (char *) node->name,
                    (char *) original_node->name,
                    KRFFS_FILE_NAME_BUFFER_SIZE
                );
                node->name[KRFFS_FILE_NAME_BUFFER_SIZE - 1] = '\0';

                node->id =
                    original_node->id;
                node->mode =
                    original_node->mode;
                node->uid =
                    original_node->uid;
                node->gid =
                    original_node->gid;
                node->access_time =
                    original_node->access_time;
                node->modification_time =
                    original_node->modification_time;
                node->attribute_change_time =
                    original_node->attribute_change_time;

                krffs_sync_mapping(
                    node,
                    sizeof(*node)
                );

                uint64_t data_size = original_node->data_size;
                if (data_size > 0) {
                    void *source =
                        original_node + 1;
                    void *destination =
                        node + 1;

                    memcpy(
                        destination,
                        source,
                        data_size
                    );

                    krffs_sync_mapping(
                        destination,
                        data_size
                    );
                }
            }

            result =
                node;
        }
    } while(!result &&
                (node =
                     krffs_get_next_node(
                         file_system,
                         node
                     )) != file_system->node);

    return result;
}

struct krffs_node *krffs_resize_reserved_node(
                       struct krffs_file_system *file_system,
                       struct krffs_node *node,
                       uint64_t size
                   )
{
    struct krffs_node *result = NULL;

    if (!file_system || !file_system->node) {
        return result;
    }

    if (!node || node->type != KRFFS_Reserved_Node) {
        return result;
    }

    uint64_t size_with_header =
        sizeof(struct krffs_node) + size;

    /* node has the same size, nothing to do here */
    if (node->size == size_with_header) {
        return node;
    }

    /* get the next node */
    struct krffs_node *next_node =
        krffs_get_next_node(
            file_system,
            node
        );

    /* attempt to increase or decrease the size of the block */
    if (size_with_header > node->size) {
        /* increase the block size */
        uint64_t difference =
            size_with_header - node->size;

        if (next_node != file_system->node &&
                next_node->type == KRFFS_Free_Node &&
                    next_node->size >= difference) {
            /* extend the block by using empty space from the next free block */
            node->size +=
                difference;
            node->data_size =
                size;

            krffs_sync_mapping(
                node,
                sizeof(*node)
            );

            result =
                node;

            uint64_t node_size =
                node->size;
            uint64_t next_node_size =
                next_node->size - difference;

            if (next_node_size > 0) {
                if (next_node_size < sizeof(*node)) {
                    /* not enough space for a header, extend the reserved
                       node */
                    node->size +=
                        next_node_size;

                    krffs_sync_mapping(
                        node,
                        sizeof(*node)
                    );
                } else {
                    /* if the next block is larger than the required new space,
                     * resize it */
                    node =
                        krffs_get_next_node(
                            file_system,
                            node
                        );

                    krffs_initialize_free_node(
                        node,
                        next_node_size,
                        node_size
                    );
                }
            }

            next_node =
                krffs_get_next_node(
                    file_system,
                    node
                );

            next_node->previous_node_size =
                node->size;

            krffs_sync_mapping(
                next_node,
                sizeof(*next_node)
            );
        } else {
            /* if not enough space, attempt to relocate the block */
            result =
                krffs_allocate_reserved_node(
                    file_system,
                    size,
                    node
                );

            /* remove the old block on success */
            if (result) {
                krffs_remove_reserved_node(
                    file_system,
                    node
                );
            }
        }
    } else if (size_with_header < node->size) {
        /* decrease the block size */
        uint64_t difference =
            node->size - size_with_header;

        node->size =
            size_with_header;
        node->data_size =
            size;

        krffs_sync_mapping(
            node,
            sizeof(*node)
        );

        result =
            node;

        if (next_node->type == KRFFS_Free_Node) {
            /* combine the new empty space with the free block next to it */
            node =
                krffs_get_next_node(
                    file_system,
                    node
                );

            krffs_initialize_free_node(
                node,
                difference + next_node->size,
                size_with_header
            );

            next_node =
                krffs_get_next_node(
                    file_system,
                    node
                );
        } else {
            if (difference < sizeof(*node)) {
                /* not enough space for a header, extend the reserved node
                   back */
                node->size +=
                    difference;

                krffs_sync_mapping(
                    node,
                    sizeof(*node)
                );
            } else {
                /* next block is used, create a free block from the empty
                   space */
                node =
                    krffs_get_next_node(
                        file_system,
                        node
                    );

                krffs_initialize_free_node(
                    node,
                    difference,
                    size_with_header
                );
            }
        }

        next_node->previous_node_size =
            node->size;

        krffs_sync_mapping(
            next_node,
            sizeof(*next_node)
        );
    }

    return result;
}

void krffs_remove_reserved_node(
         struct krffs_file_system *file_system,
         struct krffs_node *node
     )
{
    if (!file_system || !file_system->node) {
        return;
    }

    if (!node || node->type != KRFFS_Reserved_Node) {
        return;
    }

    /* get the previous node */
    struct krffs_node *previous_node = NULL;
    if (node != file_system->node) {
        previous_node =
            krffs_get_previous_node(
                file_system,
                node
            );
    }

    /* get the next node */
    struct krffs_node *next_node =
        krffs_get_next_node(
            file_system,
            node
        );

    if (next_node == file_system->node) {
        next_node = NULL;
    }

    /* mark the block as free */
    node->type =
        KRFFS_Free_Node;

    krffs_sync_mapping(
        node,
        sizeof(*node)
    );

    bool size_was_changed = false;

    /* attempt to combine with the next free block */
    if (next_node && next_node->type == KRFFS_Free_Node) {
        node->size +=
            next_node->size;

        krffs_sync_mapping(
            node,
            sizeof(*node)
        );

        size_was_changed = true;
    }

    /* attempt to combine with the previous free block */
    if (previous_node && previous_node->type == KRFFS_Free_Node) {
        previous_node->size +=
            node->size;
        node =
            previous_node;

        krffs_sync_mapping(
            node,
            sizeof(*node)
        );

        size_was_changed = true;
    }

    /* if neccessary, update the previous size for the next block */
    if (size_was_changed) {
        next_node =
            krffs_get_next_node(
                file_system,
                node
            );
        next_node->previous_node_size =
            node->size;

        krffs_sync_mapping(
            next_node,
            sizeof(*next_node)
        );
    }
}
