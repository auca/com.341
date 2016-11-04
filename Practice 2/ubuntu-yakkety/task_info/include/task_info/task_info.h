#ifndef TASK_INFO_H
#define TASK_INFO_H

#include <linux/types.h>

#define TASK_INFO_NAME_LENGTH 32

struct task_info
{
        /* Process identification */
        int32_t pid;  /* Process ID */
        char name[TASK_INFO_NAME_LENGTH];

        /* Process owners */
        int32_t ppid; /* Parent Process ID */
        uint32_t uid; /* User ID           */
        uint32_t gid; /* Group ID          */

        /* Scheduling information */
        uint32_t state;
        int32_t nice_level; /* process priority */

        /* CPU usage information (in usec) */
        uint64_t user_cpu_time;          /* CPU time spent in the user space        */
        uint64_t system_cpu_time;        /* CPU time spent in the kernel space      */
        uint64_t user_cpu_time_scaled;   /* user CPU time scaled by CPU frequency   */
        uint64_t system_cpu_time_scaled; /* system CPU time scaled by CPU frequency */

        /* Memory usage information */
        uint64_t core_memory_bytes_used;
        uint64_t virtual_memory_bytes_used;
        /* Page fault statistics */
        uint64_t minor_page_fault_count;
        uint64_t major_page_fault_count;

        /* I/O statistics */
        uint64_t bytes_read;
        uint64_t bytes_written;
        uint64_t read_syscalls_count;
        uint64_t write_syscalls_count;
};

#endif

