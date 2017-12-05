#ifndef KRFFS_FUSE_OPERATIONS_H
#define KRFFS_FUSE_OPERATIONS_H

#include "krffs_platform.h"

#include <fuse.h>

/*
    Called by FUSE to return file attributes for a file at `path`.
 */
int krffs_getattr(
        const char *path,
        struct PLATFORM_STAT *stat_buffer
    );

/*
    Called by FUSE to return a list of files and directories at `path`.
 */
int krffs_readdir(
        const char *path,
        void *buffer,
        fuse_fill_dir_t filler,
        PLATFORM_OFF_T offset,
        struct fuse_file_info *file_info
    );

/*
    Called by FUSE to open a file and get a file descriptor for a `path`.
 */
int krffs_open(
        const char *path,
        struct fuse_file_info *file_info
    );

/*
    Called by FUSE to read data of `size` from an opened file at `path` into a
    `buffer` starting from `offset`.
 */
int krffs_read(
        const char *path,
        char *buffer,
        size_t size,
        PLATFORM_OFF_T offset,
        struct fuse_file_info *file_info
    );

/*
    Called by FUSE to create a file node (anything but a directory or a link) at `path`.
 */
int krffs_mknod(
        const char *path,
        mode_t mode,
        dev_t device
    );

/*
    Called by FUSE to create a regular file at `path` and open it.
 */
int krffs_create(
        const char *path,
        mode_t mode,
        struct fuse_file_info *file_info
    );

/*
    Called by FUSE to write a `buffer` of `size` starting from `offset` at
    `path`.
 */
int krffs_write(
        const char *path,
        const char *buffer,
        size_t size,
        PLATFORM_OFF_T offset,
        struct fuse_file_info *file_info
    );

/*
    Called by FUSE to flush file buffers for a metadata block or file
    contents at `path`.
 */
int krffs_fsync(
        const char *path,
        int is_data_sync,
        struct fuse_file_info *file_info
    );

/*
    Called by FUSE to change the size of a file to `new_size` at `path`.
 */
int krffs_truncate(
        const char *path,
        PLATFORM_OFF_T new_size
    );

/*
    Called by FUSE to rename or move a file from `path` to `new_path`.
 */
int krffs_rename(
        const char *path,
        const char *new_path
    );

/*
    Called by FUSE to remove a file at `path`.
 */
int krffs_unlink(const char *path);

/*
    Called by FUSE to update access and modification time for `path`.
 */
int krffs_utimens(
        const char *path,
        const struct timespec time[2]
    );

/*
    Called by FUSE to set file permissions for `path`.
 */
int krffs_chmod(
        const char *path,
        mode_t mode
    );

/*
    Called by FUSE to change a file owner or group for `path`.
 */
int krffs_chown(
        const char *path,
        uid_t uid,
        gid_t gid
    );

/*
    Called by FUSE to get file system information used to store a file at
    `path`.
 */
int krffs_statfs(
        const char *path,
        struct statvfs *statvfs_buffer
    );

/*
    Functions pointers to our file system handlers that will be called by FUSE.
 */
static struct fuse_operations krffs_operations = {
    .getattr  = krffs_getattr,
    .readdir  = krffs_readdir,
    .open     = krffs_open,
    .read     = krffs_read,
    .mknod    = krffs_mknod,
    .create   = krffs_create,
    .write    = krffs_write,
    .fsync    = krffs_fsync,
    .truncate = krffs_truncate,
    .unlink   = krffs_unlink,
    .rename   = krffs_rename,
    .utimens  = krffs_utimens,
    .chmod    = krffs_chmod,
    .chown    = krffs_chown,
    .statfs   = krffs_statfs
};

#endif
