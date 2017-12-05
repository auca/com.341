#include "krffs_node.h"

#include "krffs_file_system.h"
#include "krffs_platform.h"
#include "krffs_utilities.h"

#include <assert.h>
#include <string.h>

struct krffs_node_time krffs_get_current_node_time()
{
    struct timeval time_of_day;
    krffs_get_time_of_day(&time_of_day);

    struct krffs_node_time node_time;
    node_time.seconds =
        time_of_day.tv_sec;
    node_time.nanoseconds =
        time_of_day.tv_usec * 1000;

    return node_time;
}

time_t krffs_get_time_from_node_time(
           const struct krffs_node_time *node_time
       )
{
    assert(node_time);

    return (time_t) node_time->seconds;
}

struct timespec krffs_get_timespec_from_node_time(
           const struct krffs_node_time *node_time
       )
{
    assert(node_time);

    struct timespec time;
    time.tv_sec =
        node_time->seconds;
    time.tv_nsec =
        node_time->nanoseconds;

    return time;
}

struct krffs_node *krffs_initialize_free_node(
                       struct krffs_node *node,
                       uint64_t size,
                       uint64_t previous_node_size
                   )
{
    assert(node);

    node->magic =
        KRFFS_File_System_Magic;
    node->type =
        KRFFS_Free_Node;
    node->size =
        size;
    node->previous_node_size =
        previous_node_size;
    node->data_size =
        0;

    krffs_sync_mapping(
        node,
        sizeof(*node)
    );

    return node;
}

struct krffs_node *krffs_wrap_node_position(
                       struct krffs_file_system *file_system,
                       struct krffs_node *node
                   )
{
    assert(file_system && node);

    uint64_t position =
        (uint64_t) node;
    uint64_t base =
        (uint64_t) file_system->node;
    uint64_t size =
        file_system->size;
    uint64_t limit =
        base + size;

    if (position >= limit) {
        position -=
            size;
    } else if (position < base) {
        position +=
            size;
    }

    return (struct krffs_node *) position;
}

struct krffs_node *krffs_initialize_root_node(
                       struct krffs_node *node,
                       uint64_t size,
                       uint64_t previous_node_size
                   )
{
    assert(node);

    node->magic =
        KRFFS_File_System_Magic;
    node->type =
        KRFFS_Root_Node;
    node->id =
        krffs_calculate_djb_hash(
            (uint8_t *) KRFFS_File_System_Root_Node_Name
        );

    strncpy(
        (char *) node->name,
        KRFFS_File_System_Root_Node_Name,
        KRFFS_FILE_NAME_BUFFER_SIZE
    );
    node->name[KRFFS_FILE_NAME_BUFFER_SIZE - 1] = '\0';

    node->mode =
        KRFFS_File_System_Root_Node_Mode;

#ifdef WINDOWS
    node->uid = 0;
    node->gid = 0;
#else
    node->uid =
        geteuid();
    node->gid =
        getegid();
#endif

    struct krffs_node_time current_node_time =
        krffs_get_current_node_time();

    node->access_time =
        current_node_time;
    node->modification_time =
        current_node_time;
    node->attribute_change_time =
        current_node_time;

    node->size =
        size;
    node->previous_node_size =
        previous_node_size;
    node->data_size =
        0;

    return node;
}

void krffs_set_reserved_node_name(
         struct krffs_node *node,
         const char *name
     )
{
    assert(node && name);

    strncpy(
        (char *) node->name,
        name,
        KRFFS_FILE_NAME_BUFFER_SIZE
    );
    node->name[KRFFS_FILE_NAME_BUFFER_SIZE - 1] = '\0';

    node->id =
        krffs_calculate_djb_hash(
            node->name
        );

    krffs_sync_mapping(
        node,
        sizeof(*node)
    );
}

struct krffs_node *krffs_get_previous_node(
                       struct krffs_file_system *file_system,
                       struct krffs_node *node
                   )
{
    assert(file_system && node);

    struct krffs_node *previous_node =
        krffs_wrap_node_position(
            file_system,
            (struct krffs_node *) (
                ((char *) node) -
                    node->previous_node_size
            )
        );

    return previous_node;
}

struct krffs_node *krffs_get_next_node(
                       struct krffs_file_system *file_system,
                       struct krffs_node *node
                   )
{
    assert(file_system && node);

    struct krffs_node *next_node =
        krffs_wrap_node_position(
            file_system,
            (struct krffs_node *) (
                ((char *) node) +
                    node->size
            )
        );

    return next_node;
}

uint64_t krffs_get_node_relative_position(
             struct krffs_file_system *file_system,
             struct krffs_node *node
         )
{
    assert(file_system && node);

    uint64_t position =
        (uint64_t) node;
    uint64_t base =
        (uint64_t) file_system->node;
    uint64_t relative_position =
        position - base;

    return relative_position;
}

bool krffs_is_node_in_file_system(
         struct krffs_file_system *file_system,
         struct krffs_node *node
     )
{
    assert(file_system && node);

    uint64_t position =
        (uint64_t) node;
    uint64_t base =
        (uint64_t) file_system->node;
    uint64_t size =
        file_system->size;
    uint64_t limit =
        base + size;

    bool is_inside =
        position >= base &&
            position < limit;

    return is_inside;
}

struct krffs_node *krffs_find_reserved_node_by_name(
                       struct krffs_file_system *file_system,
                       const char *name
                   )
{
    assert(file_system && name);

    struct krffs_node *result = NULL;

    uint64_t hash =
        krffs_calculate_djb_hash((uint8_t *) name);

    struct krffs_node *node = file_system->node;
    do {
        if (node->type == KRFFS_Reserved_Node &&
                node->id == hash) {
            result =
                node;

            break;
        }
    } while ((node =
                  krffs_get_next_node(
                      file_system,
                      node
                  )) != file_system->node);

    return result;
}

struct krffs_node *krffs_find_reserved_or_root_node_by_name(
                       struct krffs_file_system *file_system,
                       const char *name
                   )
{
    assert(file_system && name);

    struct krffs_node *result = NULL;

    uint64_t hash =
        krffs_calculate_djb_hash((uint8_t *) name);

    struct krffs_node *node = file_system->node;
    do {
        if ((node->type == KRFFS_Reserved_Node ||
             node->type == KRFFS_Root_Node) &&
                node->id == hash) {
            result =
                node;

            break;
        }
    } while ((node =
                  krffs_get_next_node(
                      file_system,
                      node
                  )) != file_system->node);

    return result;
}

size_t krffs_count_free_nodes(
           struct krffs_file_system *file_system
       )
{
    size_t result = 0;

    struct krffs_node *node = file_system->node;
    do {
        if (node->type == KRFFS_Free_Node) {
            ++result;
        }
    } while ((node =
                  krffs_get_next_node(
                      file_system,
                      node
                  )) != file_system->node);

    return result;
}

uint64_t krffs_calculate_total_free_space(
             struct krffs_file_system *file_system
         )
{
    uint64_t result = 0;

    struct krffs_node *node = file_system->node;
    do {
        if (node->type == KRFFS_Free_Node) {
            result +=
                node->size -
                    sizeof(*node);
        }
    } while ((node =
                  krffs_get_next_node(
                      file_system,
                      node
                  )) != file_system->node);

    return result;
}

size_t krffs_count_reserved_nodes(
           struct krffs_file_system *file_system
       )
{
    size_t result = 0;

    struct krffs_node *node = file_system->node;
    do {
        if (node->type == KRFFS_Reserved_Node) {
            ++result;
        }
    } while ((node =
                  krffs_get_next_node(
                      file_system,
                      node
                  )) != file_system->node);

    return result;
}

uint64_t krffs_calculate_total_reserved_space(
             struct krffs_file_system *file_system
         )
{
    uint64_t result = 0;

    struct krffs_node *node = file_system->node;
    do {
        if (node->type == KRFFS_Reserved_Node) {
            result +=
                node->size -
                    sizeof(*node);
        }
    } while ((node =
                  krffs_get_next_node(
                      file_system,
                      node
                  )) != file_system->node);

    return result;
}
