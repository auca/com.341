#include "krffs_fuse_operations.h"

#include "krffs_allocator.h"
#include "krffs_file_system.h"
#include "krffs_node.h"
#include "krffs_platform.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

extern struct krffs_file_system *KRFFS_File_System;

static inline void fill_stat_buffer_for_node(
                       struct PLATFORM_STAT *stat_buffer,
                       struct krffs_node *node,
                       uint16_t file_type,
                       uint16_t hard_links_count
                   )
{
    stat_buffer->st_nlink =
        hard_links_count;
    stat_buffer->st_size =
        node->data_size;
    stat_buffer->st_mode =
        file_type | node->mode;
    stat_buffer->st_uid =
        node->uid;
    stat_buffer->st_gid =
        node->gid;
#if defined(APPLE) && (!defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE))
    stat_buffer->st_atimespec =
        krffs_get_timespec_from_node_time(
            &node->access_time
        );
    stat_buffer->st_mtimespec =
        krffs_get_timespec_from_node_time(
            &node->modification_time
        );
    stat_buffer->st_ctimespec =
        krffs_get_timespec_from_node_time(
            &node->attribute_change_time
        );
#elif defined(WINDOWS)
    stat_buffer->st_atim =
        krffs_get_timespec_from_node_time(
            &node->access_time
        );
    stat_buffer->st_mtim =
        krffs_get_timespec_from_node_time(
            &node->modification_time
        );
    stat_buffer->st_ctim =
        krffs_get_timespec_from_node_time(
            &node->attribute_change_time
        );
#else
    stat_buffer->st_atime =
        krffs_get_time_from_node_time(
            &node->access_time
        );
    stat_buffer->st_mtime =
        krffs_get_time_from_node_time(
            &node->modification_time
        );
    stat_buffer->st_ctime =
        krffs_get_time_from_node_time(
            &node->attribute_change_time
        );
#endif
}

int krffs_getattr(
        const char *path,
        struct PLATFORM_STAT *stat_buffer
    )
{
    int result = 0;

    memset(
        stat_buffer,
        0,
        sizeof(*stat_buffer)
    );

    if (!KRFFS_File_System) {
        fprintf(
            stderr,
            "No file system to work with.\n"
        );

        result =
            -ENOENT;
    } else {
        struct krffs_node *root_node = KRFFS_File_System->node;
        if (!root_node) {
            fprintf(
                stderr,
                "The root node '%s' was not found.\n",
                path
            );

            result =
                -ENOENT;
        } else {
            ++path;

            struct krffs_node *node =
                krffs_find_reserved_or_root_node_by_name(
                    KRFFS_File_System,
                    path
                );

            if (!node) {
                fprintf(
                    stderr,
                    "The file '%s' was not found.\n",
                    path
                );

                result =
                    -ENOENT;
            } else {
                if (node->type == KRFFS_Root_Node) {
                    fill_stat_buffer_for_node(
                        stat_buffer,
                        root_node,
                        S_IFDIR,
                        2
                    );
                } else {
                    fill_stat_buffer_for_node(
                        stat_buffer,
                        node,
                        S_IFREG,
                        1
                    );
                }
            }
        }
    }

    return result;
}

int krffs_readdir(
        const char *path,
        void *buffer,
        fuse_fill_dir_t filler,
        PLATFORM_OFF_T offset,
        struct fuse_file_info *file_info
    )
{
    int result = 0;

    if (!KRFFS_File_System) {
        fprintf(
            stderr,
            "No file system to work with.\n"
        );

        result =
            -ENOENT;
    } else {
        struct krffs_node *root_node = KRFFS_File_System->node;
        if (!root_node) {
            fprintf(
                stderr,
                "The root node '%s' was not found.\n",
                path
            );

            result =
                -ENOENT;
        } else {
            ++path;

            if (strcmp((char *) root_node->name, path) != 0) {
                fprintf(
                    stderr,
                    "This file system does not support directories. "
                    "Invalid path '%s'.\n",
                    path
                );

                result =
                    -ENOENT;
            } else {
                filler(buffer, ".",  NULL, 0);
                filler(buffer, "..", NULL, 0);

                struct krffs_node *node = KRFFS_File_System->node;
                do {
                    if (node->type == KRFFS_Reserved_Node) {
                        filler(
                            buffer,
                            (const char *) node->name,
                            NULL, 0
                        );
                    }
                } while ((node =
                              krffs_get_next_node(
                                  KRFFS_File_System,
                                  node
                              )) != KRFFS_File_System->node);
            }
        }
    }

    return result;
}

int krffs_open(
        const char *path,
        struct fuse_file_info *file_info
    )
{
    int result = 0;

    if (!KRFFS_File_System) {
        fprintf(
            stderr,
            "No file system to work with.\n"
        );

        result =
            -ENOENT;
    } else {
        ++path;

        struct krffs_node *node =
            krffs_find_reserved_node_by_name(
                KRFFS_File_System,
                path
            );

        if (!node) {
            fprintf(
                stderr,
                "The file '%s' was not found.\n",
                path
            );

            result =
                -ENOENT;
        }
    }

    return result;
}

int krffs_read(
        const char *path,
        char *buffer,
        size_t size,
        PLATFORM_OFF_T offset,
        struct fuse_file_info *file_info
    )
{
    int result = 0;

    if (!KRFFS_File_System) {
        fprintf(
            stderr,
            "No file system to work with.\n"
        );

        result =
            -ENOENT;
    } else {
        ++path;

        struct krffs_node *node =
            krffs_find_reserved_node_by_name(
                KRFFS_File_System,
                path
            );

        if (!node) {
            fprintf(
                stderr,
                "The file '%s' was not found.\n",
                path
            );

            result =
                -ENOENT;
        } else {
            void *base =
                node + 1;
            uint64_t data_size =
                node->data_size;

            if (offset < data_size) {
                if (offset + size > data_size) {
                    size =
                        data_size - offset;
                }

                memcpy(
                    buffer,
                    ((char *) base) + offset,
                    size
                );

                node->access_time =
                    krffs_get_current_node_time();
            } else {
                size = 0;
            }

            result =
                (int) size;
        }
    }

    return result;
}

int krffs_mknod(
        const char *path,
        mode_t mode,
        dev_t device
    )
{
    int result = 0;

    if (!KRFFS_File_System) {
        fprintf(
            stderr,
            "No file system to work with.\n"
        );

        result =
            -ENOENT;
    } else {
        struct krffs_node *root_node = KRFFS_File_System->node;
        if (!root_node) {
            fprintf(
                stderr,
                "The root node '%s' was not found.\n",
                path
            );

            result =
                -ENOENT;
        } else {
            ++path;

            struct krffs_node *node =
                krffs_find_reserved_node_by_name(
                    KRFFS_File_System,
                    path
                );

            if (node) {
                fprintf(
                    stderr,
                    "The file '%s' already exists.\n",
                    path
                );

                result =
                    -EEXIST;
            } else {
                node = krffs_allocate_reserved_node(
                    KRFFS_File_System,
                    0,
                    NULL
                );

                if (!node) {
                    fprintf(
                        stderr,
                        "Not enough space for a new file at '%s'.\n",
                        path
                    );

                    result =
                        -ENOSPC;
                } else {
                    krffs_set_reserved_node_name(
                        node,
                        path
                    );

                    node->mode =
                        mode;

                    struct fuse_context *context = fuse_get_context();
                    if (context) {
                        node->uid =
                            context->uid;
                        node->gid =
                            context->gid;
                    }

                    struct krffs_node_time current_node_time =
                        krffs_get_current_node_time();

                    node->access_time =
                        current_node_time;
                    node->modification_time =
                        current_node_time;
                    node->attribute_change_time =
                        current_node_time;

                    root_node->modification_time =
                        current_node_time;
                    root_node->attribute_change_time =
                        current_node_time;
                }
            }
        }
    }

    return result;
}

int krffs_create(
        const char *path,
        mode_t mode,
        struct fuse_file_info *file_info
    )
{
    int result = 0;

    if (!KRFFS_File_System) {
        fprintf(
            stderr,
            "No file system to work with.\n"
        );

        result =
            -ENOENT;
    } else {
        struct krffs_node *root_node = KRFFS_File_System->node;
        if (!root_node) {
            fprintf(
                stderr,
                "The root node '%s' was not found.\n",
                path
            );

            result =
                -ENOENT;
        } else {
            ++path;

            struct krffs_node *node =
                krffs_find_reserved_node_by_name(
                    KRFFS_File_System,
                    path
                );

            if (!node) {
                node = krffs_allocate_reserved_node(
                    KRFFS_File_System,
                    0,
                    NULL
                );

                if (!node) {
                    fprintf(
                        stderr,
                        "Not enough space for a new file at '%s'.\n",
                        path
                    );

                    result =
                        -ENOSPC;
                } else {
                    krffs_set_reserved_node_name(
                        node,
                        path
                    );

                    node->mode =
                        mode;

                    struct fuse_context *context = fuse_get_context();
                    if (context) {
                        node->uid =
                            context->uid;
                        node->gid =
                            context->gid;
                    }

                    struct krffs_node_time current_node_time =
                        krffs_get_current_node_time();

                    node->access_time =
                        current_node_time;
                    node->modification_time =
                        current_node_time;
                    node->attribute_change_time =
                        current_node_time;

                    root_node->modification_time =
                        current_node_time;
                    root_node->attribute_change_time =
                        current_node_time;
                }
            }
        }
    }

    return result;
}

int krffs_write(
        const char *path,
        const char *buffer,
        size_t size,
        PLATFORM_OFF_T offset,
        struct fuse_file_info *file_info
    )
{
    int result = 0;

    if (!KRFFS_File_System) {
        fprintf(
            stderr,
            "No file system to work with.\n"
        );

        result =
            -ENOENT;
    } else {
        ++path;

        struct krffs_node *node =
            krffs_find_reserved_node_by_name(
                KRFFS_File_System,
                path
            );

        if (!node) {
            fprintf(
                stderr,
                "The file '%s' was not found.\n",
                path
            );

            result =
                -ENOENT;
        } else {
            void *base =
                node + 1;
            uint64_t available_data_size =
                node->size - sizeof(*node);

            uint64_t new_size = offset + size;
            if (new_size <= available_data_size) {
                if (new_size > node->data_size) {
                    node->data_size =
                        new_size;
                }

                krffs_sync_mapping(
                    node,
                    sizeof(*node)
                );

                void *destination =
                    ((char *) base) + offset;

                memcpy(
                    destination,
                    buffer,
                    size
                );

                krffs_sync_mapping(
                    destination,
                    size
                );

                node->modification_time =
                    krffs_get_current_node_time();

                result =
                    (int) size;
            } else {
                node =
                    krffs_resize_reserved_node(
                        KRFFS_File_System,
                        node,
                        new_size
                    );

                if (!node) {
                    fprintf(
                        stderr,
                        "Not enough space for a new file at '%s'.\n",
                        path
                    );

                    result =
                        -ENOSPC;
                } else {
                    base =
                        node + 1;

                    void *destination =
                        ((char *) base) + offset;

                    memcpy(
                        destination,
                        buffer,
                        size
                    );

                    krffs_sync_mapping(
                        destination,
                        size
                    );

                    node->modification_time =
                        krffs_get_current_node_time();

                    result =
                        (int) size;
                }
            }
        }
    }

    return result;
}

int krffs_fsync(
        const char *path,
        int is_data_sync,
        struct fuse_file_info *file_info
    )
{
    int result = 0;

    if (!KRFFS_File_System) {
        fprintf(
            stderr,
            "No file system to work with.\n"
        );

        result =
            -ENOENT;
    } else {
        ++path;

        struct krffs_node *node =
            krffs_find_reserved_or_root_node_by_name(
                KRFFS_File_System,
                path
            );

        if (!node) {
            fprintf(
                stderr,
                "The file '%s' was not found.\n",
                path
            );

            result =
                -ENOENT;
        }

        /* Do nothing... We don't buffer data. */
    }

    return result;
}

int krffs_truncate(
        const char *path,
        PLATFORM_OFF_T new_size
    )
{
    int result = 0;

    if (!KRFFS_File_System) {
        fprintf(
            stderr,
            "No file system to work with.\n"
        );

        result =
            -ENOENT;
    } else {
        ++path;

        struct krffs_node *node =
            krffs_find_reserved_node_by_name(
                KRFFS_File_System,
                path
            );

        if (!node) {
            fprintf(
                stderr,
                "The file '%s' was not found.\n",
                path
            );

            result =
                -ENOENT;
        } else {
            uint64_t data_size =
                node->data_size;

            node =
                krffs_resize_reserved_node(
                    KRFFS_File_System,
                    node,
                    new_size
                );

            if (!node) {
                fprintf(
                    stderr,
                    "Not enough space for a new file at '%s'.\n",
                    path
                );

                result =
                    -ENOSPC;
            } else {
                if (new_size > data_size) {
                    void *base =
                        node + 1;

                    void *destination =
                        ((char *) base) + data_size;
                    uint64_t size =
                        new_size - data_size;

                    memset(
                        destination,
                        0,
                        size
                    );

                    krffs_sync_mapping(
                        destination,
                        size
                    );
                }

                node->modification_time =
                    krffs_get_current_node_time();
            }
        }
    }

    return result;
}

int krffs_rename(
        const char *path,
        const char *new_path
    )
{
    int result = 0;

    if (!KRFFS_File_System) {
        fprintf(
            stderr,
            "No file system to work with.\n"
        );

        result =
            -ENOENT;
    } else {
        struct krffs_node *root_node = KRFFS_File_System->node;
        if (!root_node) {
            fprintf(
                stderr,
                "The root node '%s' was not found.\n",
                path
            );

            result =
                -ENOENT;
        } else {
            ++path;
            ++new_path;

            struct krffs_node *node_at_path =
                krffs_find_reserved_node_by_name(
                    KRFFS_File_System,
                    path
                );

            if (!node_at_path) {
                fprintf(
                    stderr,
                    "The file '%s' was not found.\n",
                    path
                );

                result =
                    -ENOENT;
            } else {
                struct krffs_node *node_at_new_path =
                    krffs_find_reserved_node_by_name(
                        KRFFS_File_System,
                        new_path
                    );

                if (node_at_new_path) {
                    krffs_remove_reserved_node(
                        KRFFS_File_System,
                        node_at_new_path
                    );
                }

                krffs_set_reserved_node_name(
                    node_at_path,
                    new_path
                );

                struct krffs_node_time current_node_time =
                    krffs_get_current_node_time();

                root_node->modification_time =
                    current_node_time;
                root_node->attribute_change_time =
                    current_node_time;
            }
        }
    }

    return result;
}

int krffs_unlink(const char *path)
{
    int result = 0;

    if (!KRFFS_File_System) {
        fprintf(
            stderr,
            "No file system to work with.\n"
        );

        result =
            -ENOENT;
    } else {
        struct krffs_node *root_node = KRFFS_File_System->node;
        if (!root_node) {
            fprintf(
                stderr,
                "The root node '%s' was not found.\n",
                path
            );

            result =
                -ENOENT;
        } else {
            ++path;

            struct krffs_node *node =
                krffs_find_reserved_node_by_name(
                    KRFFS_File_System,
                    path
                );

            if (!node) {
                fprintf(
                    stderr,
                    "The file '%s' was not found.\n",
                    path
                );

                result =
                    -ENOENT;
            } else {
                krffs_remove_reserved_node(
                    KRFFS_File_System,
                    node
                );

                struct krffs_node_time current_node_time =
                    krffs_get_current_node_time();

                root_node->modification_time =
                    current_node_time;
                root_node->attribute_change_time =
                    current_node_time;
            }
        }
    }

    return result;
}

int krffs_utimens(
        const char *path,
        const struct timespec time[2]
    )
{
    int result = 0;

    if (!KRFFS_File_System) {
        fprintf(
            stderr,
            "No file system to work with.\n"
        );

        result =
            -ENOENT;
    } else {
        ++path;

        struct krffs_node *node =
            krffs_find_reserved_or_root_node_by_name(
                KRFFS_File_System,
                path
            );

        if (!node) {
            fprintf(
                stderr,
                "The file '%s' was not found.\n",
                path
            );

            result =
                -ENOENT;
        } else {
            struct krffs_node_time current_node_time =
                krffs_get_current_node_time();

            node->access_time =
                current_node_time;
            node->modification_time =
                current_node_time;

            krffs_sync_mapping(
                node,
                sizeof(*node)
            );
        }
    }

    return result;
}

int krffs_chmod(
        const char *path,
        mode_t mode
    )
{
    int result = 0;

    if (!KRFFS_File_System) {
        fprintf(
            stderr,
            "No file system to work with.\n"
        );

        result =
            -ENOENT;
    } else {
        ++path;

        struct krffs_node *node =
            krffs_find_reserved_or_root_node_by_name(
                KRFFS_File_System,
                path
            );

        if (!node) {
            fprintf(
                stderr,
                "The file '%s' was not found.\n",
                path
            );

            result =
                -ENOENT;
        } else {
            if (node->type == KRFFS_Root_Node) {
                node->mode =
                    S_IFDIR | mode;
            } else {
                node->mode =
                    S_IFREG | mode;
            }
            node->attribute_change_time =
                krffs_get_current_node_time();

            krffs_sync_mapping(
                node,
                sizeof(*node)
            );
        }
    }

    return result;
}

int krffs_chown(
        const char *path,
        uid_t uid,
        gid_t gid
    )
{
    int result = 0;

    if (!KRFFS_File_System) {
        fprintf(
            stderr,
            "No file system to work with.\n"
        );

        result =
            -ENOENT;
    } else {
        ++path;

        struct krffs_node *node =
            krffs_find_reserved_or_root_node_by_name(
                KRFFS_File_System,
                path
            );

        if (!node) {
            fprintf(
                stderr,
                "The file '%s' was not found.\n",
                path
            );

            result =
                -ENOENT;
        } else {
            node->uid =
                uid;
            node->gid =
                gid;
            node->attribute_change_time =
                krffs_get_current_node_time();

            krffs_sync_mapping(
                node,
                sizeof(*node)
            );
        }
    }

    return result;
}

int krffs_statfs(
        const char *path,
        struct statvfs *statvfs_buffer
    )
{
    int result = 0;

    memset(
        statvfs_buffer,
        0,
        sizeof(*statvfs_buffer)
    );

    if (!KRFFS_File_System) {
        fprintf(
            stderr,
            "No file system to work with.\n"
        );

        result =
            -ENOENT;
    } else {
        if (!path) {
            result =
                -EFAULT;
        } else {
            static const unsigned long StatFS_Block_Size =
                1024;

            uint64_t total_space =
                KRFFS_File_System->size;
            uint64_t total_free_space =
                krffs_calculate_total_free_space(
                    KRFFS_File_System
                );
            size_t reserved_node_count =
                krffs_count_reserved_nodes(
                    KRFFS_File_System
                );

            statvfs_buffer->f_bsize  =
                StatFS_Block_Size;
            statvfs_buffer->f_frsize =
                StatFS_Block_Size;
            statvfs_buffer->f_blocks =
                (fsblkcnt_t) ((total_space - 1) /
                                  StatFS_Block_Size + 1);
            statvfs_buffer->f_bfree =
                (fsblkcnt_t) ((total_free_space - 1) /
                                  StatFS_Block_Size + 1);
            statvfs_buffer->f_bavail =
                statvfs_buffer->f_bfree;
            statvfs_buffer->f_files =
                (fsfilcnt_t) reserved_node_count;
            statvfs_buffer->f_ffree =
                0;
            statvfs_buffer->f_favail =
                statvfs_buffer->f_ffree;
            statvfs_buffer->f_namemax =
                KRFFS_FILE_NAME_BUFFER_SIZE - 1;
        }
    }

    return result;
}
