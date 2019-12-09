#ifndef KRFFS_PLATFORM_H
#define KRFFS_PLATFORM_H

#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>

#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS
#else
#define UNIX
#endif

#ifdef __APPLE__
#define APPLE
#endif

#ifdef __linux__
#define LINUX
#endif

#ifdef WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

/*
    Platform specific function prefix
 */
#ifdef WINDOWS
#define PLATFORM_PREFIX(CALL) _ ## CALL
#else
#define PLATFORM_PREFIX(CALL) CALL
#endif

/*
    Platform specific file information data types.
 */
#ifdef WINDOWS
#define PLATFORM_OFF_T FUSE_OFF_T
#define PLATFORM_STAT FUSE_STAT
#else
#define PLATFORM_OFF_T off_t
#define PLATFORM_STAT stat
#endif

/*
    Platform specific file system information data types.
 */
#ifdef WINDOWS
#define fsblkcnt_t fsblkcnt64_t
#define fsfilcnt_t fsfilcnt64_t
#endif

/*
    Windows specific file mode checks
 */
#ifdef WINDOWS
#define S_ISREG(MODE) (((MODE) & S_IFMT) == S_IFREG)
#endif

/*
	Get time of day with a microsecond precision.
 */
void krffs_get_time_of_day(struct timeval *time_of_day);

/*
    Maps file contents to pages of virtual memory. Reads or writes to reserved
    pages results in reads or writes to a file.

    Uses `mmap` on Unix/Unix-like systems and `MapViewOfFile` on Windows.
 */
void *krffs_map_file(
          int file_descriptor,
          off_t offset,
          size_t size
      );

/*
    Tells the kernel to sync data from memory into a mapped file.

    Uses `msync` on Unix/Unix-like systems and `FlushViewOfFile` on Windows.
 */
int krffs_sync_mapping(
        void *address,
        size_t size
    );

/*
    Unmaps pages mapped by a call to `krffs_map_file`.

    On windows, the size parameter is ignored and the address must point to the
    first mapped page.

    Uses `munmap` on Unix/Unix-like systems and `UnmapViewOfFile` on Windows.
 */
int krffs_unmap_file(
        void *address,
        size_t size
    );

#endif
