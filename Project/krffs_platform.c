#include "krffs_platform.h"

#ifdef WINDOWS
#include <io.h>
#else
#include <sys/mman.h>
#endif

#include <stdint.h>

void krffs_get_time_of_day(struct timeval *time_of_day)
{
#ifdef WINDOWS
    static const uint64_t epoch =
        116444736000000000ULL;

    SYSTEMTIME system_time;
    GetSystemTime(&system_time);

    FILETIME file_time;
    SystemTimeToFileTime(&system_time, &file_time);

    ULARGE_INTEGER time;
    time.LowPart =
        file_time.dwLowDateTime;
    time.HighPart =
        file_time.dwHighDateTime;

    time_of_day->tv_sec =
        (long) ((time.QuadPart - epoch) / 10000000L);
    time_of_day->tv_usec =
        (long) (system_time.wMilliseconds * 1000);
#else
    gettimeofday(time_of_day, NULL);
#endif
}

void *krffs_map_file(
          int file_descriptor,
          off_t offset,
          size_t size
      )
{
#ifdef WINDOWS
    void *result =
        (void *) -1;

    DWORD offset_high =
        (DWORD) (offset >> 32);
    DWORD offset_low =
        (DWORD) (offset & 0xFFFFFFFF);

    HANDLE file_handle =
        (HANDLE) _get_osfhandle(file_descriptor);

    HANDLE mapping_handle =
        CreateFileMapping(
            file_handle,
            NULL,
            PAGE_READWRITE,
            0,
            0,
            NULL
        );

    if (mapping_handle) {
        result =
            MapViewOfFile(
                mapping_handle,
                FILE_MAP_WRITE,
                offset_high,
                offset_low,
                size
            );

        if (result) {
            CloseHandle(
                mapping_handle
            );
        } else {
            result =
                (void *) -1;
        }
    }

    return result;
#else
    return mmap(
               NULL,
               size,
               PROT_READ | PROT_WRITE,
               MAP_SHARED,
               file_descriptor,
               offset
           );
#endif
}

int krffs_sync_mapping(
        void *address,
        size_t size
    )
{
#ifdef WINDOWS
    return FlushViewOfFile(
               address,
               size
           ) == 0 ? -1 : 0;
#else
    return msync(
               address,
               size,
               MS_SYNC
           );
#endif
}

int krffs_unmap_file(
        void *address,
        size_t size
    )
{
#ifdef WINDOWS
    return UnmapViewOfFile(
               address
           ) == 0 ? -1 : 0;
#else
    return munmap(
               address,
               size
           );
#endif
}
