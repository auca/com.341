#include <inttypes.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/sysinfo.h>
#include <sys/types.h>

#include "task_info.h"

#define HEADER_BUFFER_SIZE 1024
#define FIELD_BUFFER_SIZE 32

static const int PID_Column_Width     = 6,
                 PPID_Column_Width    = 6,
                 Name_Column_Width    = 17,
                 UID_Column_Width     = 6,
                 GID_Column_Width     = 6,
                 State_Column_Width   = 13,
                 Nice_Column_Width    = 4,
                 UTime_Column_Width   = 14,
                 KTime_Column_Width   = 14,
                 RSS_Column_Width     = 12,
                 VM_Column_Width      = 12,
                 Reads_Column_Width   = 12,
                 Writes_Column_Width  = 12,
                 Read_Column_Width    = 12,
                 Written_Column_Width = 12;

static const char *PID_Column_Name     = "PID",
                  *PPID_Column_Name    = "PPID",
                  *Name_Column_Name    = "Name",
                  *UID_Column_Name     = "UID",
                  *GID_Column_Name     = "GID",
                  *State_Column_Name   = "State",
                  *Nice_Column_Name    = "Nice",
                  *UTime_Column_Name   = "User Time",
                  *KTime_Column_Name   = "Kernel Time",
                  *RSS_Column_Name     = "RSS",
                  *VM_Column_Name      = "VM",
                  *Reads_Column_Name   = "Reads",
                  *Writes_Column_Name  = "Writes",
                  *Read_Column_Name    = "Read",
                  *Written_Column_Name = "Written";

static const int Header_Height       = 5;
static const int Memory_Column_Width = 7;

#if defined(__x86_64__)
static const int __NR_get_pids      = 329,
                 __NR_get_task_info = 330;
#elif defined(__i386__)
static const int __NR_get_pids      = 380,
                 __NR_get_task_info = 381;
#else
static const int __NR_get_pids      = 288,
                 __NR_get_task_info = 289;
#endif

static const int32_t Kernel_Thread_Daemon_PID = 2;

static const char * const Task_States[] = {
    "unknown",
    "running",
    "sleeping",
    "disk sleep",
    "stopped",
    "tracing stop",
    "dead",
    "zombie"
};
static const size_t Task_States_Count =
    sizeof(Task_States) / sizeof(*Task_States);

static const unsigned int Input_Delay = 10;

char *scale_size(uint64_t size, char *buffer, size_t buffer_size)
{
    static const int Size_Scale_Threshold = 7;

    if (!buffer) {
        return NULL;
    }

    if (buffer_size == 0) {
        return NULL;
    }

    if (snprintf(
            buffer, buffer_size,
            "%" PRIu64 " KB", size / 1024LU
        ) <= Size_Scale_Threshold) {
        return buffer;
    } else if (snprintf(
                   buffer, buffer_size,
                   "%" PRIu64 " MB", size / 1048576LU
               ) <= Size_Scale_Threshold) {
        return buffer;
    } else if (snprintf(
                   buffer, buffer_size,
                   "%" PRIu64 " GB", size / 1073741824LU
               ) <= Size_Scale_Threshold) {
        return buffer;
    } else if (snprintf(
                   buffer, buffer_size,
                   "%" PRIu64 " TB", size / 1099511627776LU
               ) <= Size_Scale_Threshold) {
        return buffer;
    } else if (snprintf(
                   buffer, buffer_size,
                   "%" PRIu64 " PB", size / 1125899906842624LU
               ) <= Size_Scale_Threshold) {
        return buffer;
    } else {
        snprintf(
            buffer, buffer_size,
            "%" PRIu64 " B ", size
        );
    }

    return buffer;
}

char *scale_time(uint64_t time, char *buffer, size_t buffer_size)
{
    static const int Time_Scale_Threshold = 7;

    if (!buffer) {
        return NULL;
    }

    if (buffer_size == 0) {
        return NULL;
    }

    if (snprintf(
            buffer, buffer_size,
            "%" PRIu64 " ms", time / 1000LU
        ) <= Time_Scale_Threshold) {
        return buffer;
    } else if (snprintf(
                   buffer, buffer_size,
                   "%" PRIu64 " s ", time / 1000000LU
               ) <= Time_Scale_Threshold) {
        return buffer;
    } else if (snprintf(
                   buffer, buffer_size,
                   "%" PRIu64 " m", time / 60000000LU
               ) <= Time_Scale_Threshold) {
        return buffer;
    } else if (snprintf(
                   buffer, buffer_size,
                   "%" PRIu64 " h", time / 3600000000LU
               ) <= Time_Scale_Threshold) {
        return buffer;
    } else if (snprintf(
                   buffer, buffer_size,
                   "%" PRIu64 " d", time / 86400000000LU
               ) <= Time_Scale_Threshold) {
        return buffer;
    } else {
        snprintf(
            buffer, buffer_size,
            "%" PRIu64 " us", time
        );
    }

    return buffer;
}

int main(int argc, char **argv)
{
        int exit_status = 0;

        char header[HEADER_BUFFER_SIZE];
        size_t header_length =
            snprintf(
                header,
                sizeof(header),
                "%-*s %-*s %-*s %-*s %-*s %-*s %*s %*s %*s "
                "%*s %*s %*s %*s %*s %*s\n",
                PID_Column_Width,     PID_Column_Name,
                PPID_Column_Width,    PPID_Column_Name,
                Name_Column_Width,    Name_Column_Name,
                UID_Column_Width,     UID_Column_Name,
                GID_Column_Width,     GID_Column_Name,
                State_Column_Width,   State_Column_Name,
                Nice_Column_Width,    Nice_Column_Name,
                UTime_Column_Width,   UTime_Column_Name,
                KTime_Column_Width,   KTime_Column_Name,
                RSS_Column_Width,     RSS_Column_Name,
                VM_Column_Width,      VM_Column_Name,
                Reads_Column_Width,   Reads_Column_Name,
                Writes_Column_Width,  Writes_Column_Name,
                Read_Column_Width,    Read_Column_Name,
                Written_Column_Width, Written_Column_Name
            );

        pid_t *pid_list = NULL;

        char total_ram_scaled[FIELD_BUFFER_SIZE];
        char used_ram_scaled[FIELD_BUFFER_SIZE];
        char free_ram_scaled[FIELD_BUFFER_SIZE];
        char total_swap_scaled[FIELD_BUFFER_SIZE];
        char used_swap_scaled[FIELD_BUFFER_SIZE];
        char free_swap_scaled[FIELD_BUFFER_SIZE];

        char user_cpu_time_scaled[FIELD_BUFFER_SIZE];
        char kernel_cpu_time_scaled[FIELD_BUFFER_SIZE];

        char core_memory_usage_scaled[FIELD_BUFFER_SIZE];
        char virtual_memory_usage_scaled[FIELD_BUFFER_SIZE];
        char data_read_scaled[FIELD_BUFFER_SIZE];
        char data_written_scaled[FIELD_BUFFER_SIZE];

        bool show_kernel_threads = false;

        if (!initscr()) {
            fprintf(
                stderr,
                "Failed to create the program's UI.\n"
            );

            exit_status =
                EXIT_FAILURE;

            goto cleanup;
        }

        if (has_colors()) {
            start_color();
            use_default_colors();
            init_pair(1, COLOR_BLACK, COLOR_WHITE);
        }
        noecho();
        halfdelay(Input_Delay);

        WINDOW *header_pad = newpad(1, header_length);
        if (!header_pad) {
            fprintf(
                stderr,
                "Failed to create a UI pad for a header.\n"
            );

            exit_status =
                EXIT_FAILURE;

            goto cleanup;
        }

        bool has_attribute = false;
        if (has_colors()) {
            wattron(header_pad, COLOR_PAIR(1));
            has_attribute =
                !has_attribute;
        }
        waddstr(header_pad, header);
        if (has_attribute) {
            wattroff(header_pad, COLOR_PAIR(1));
        }

        WINDOW *pad = NULL;
        int pad_height = -1;
        int pad_shift_y = 0;
        int pad_shift_x = 0;

        WINDOW *footer_pad = newpad(1, header_length);
        if (!footer_pad) {
            fprintf(
                stderr,
                "Failed to create a UI pad for a footer.\n"
            );

            exit_status =
                EXIT_FAILURE;

            goto cleanup;
        }

        wprintw(
            footer_pad,
            "Press 't' to %s kernel threads. Press 'q' to exit.",
            show_kernel_threads ?
                "hide" : "show"
        );

        for (;;) {
            pid_t current_pid =
                getpid();

            struct sysinfo system;
            if (sysinfo(&system)) {
                fprintf(
                    stderr,
                    "Failed to perform the 'sysinfo' system call.\n"
                );

                exit_status =
                    EXIT_FAILURE;

                goto cleanup;
            }

            long pid_list_length = syscall(__NR_get_pids, 0, NULL);
            if (pid_list_length <= 0) {
                fprintf(
                    stderr,
                    "Failed to perform the 'get_pids' system call. "
                    "Ensure that the 'task_info' subsystem was compiled "
                    "into the kernel.\n"
                );

                exit_status =
                    EXIT_FAILURE;

                goto cleanup;
            }

            size_t pid_list_size = pid_list_length * sizeof(*pid_list);
            if (!(pid_list = realloc(pid_list, pid_list_size))) {
                fprintf(stderr, "Failed to reserve memory.\n");

                exit_status =
                    EXIT_FAILURE;

                goto cleanup;
            }

            memset(pid_list, 0, pid_list_size);

            if (syscall(__NR_get_pids, pid_list_length, pid_list) <= 0) {
                fprintf(
                    stderr,
                    "Failed to perform the 'get_pids' system call. "
                    "Ensure that the 'task_info' subsystem was compiled "
                    "into the kernel.\n"
                );

                exit_status =
                    EXIT_FAILURE;

                goto cleanup;
            }

            if (pad_height != pid_list_length + 1) {
                pad_height =
                    pid_list_length + 1;

                if (pad) {
                    delwin(pad);
                }

                pad = newpad(pad_height, header_length);
                if (!pad) {
                    fprintf(
                        stderr,
                        "Failed to create a scrollable UI pad.\n"
                    );

                    exit_status =
                        EXIT_FAILURE;

                    goto cleanup;
                }

                keypad(pad, true);
            }

            size_t header_height =
                Header_Height;

            unsigned long uptime_days =
                system.uptime / 86400;
            unsigned long uptime_hours =
                system.uptime / 3600 -
                    uptime_days * 24;
            unsigned long uptime_minutes =
                system.uptime / 60 -
                    uptime_days * 1440 -
                    uptime_hours * 60;
            unsigned long uptime_seconds =
                system.uptime % 60;

            float load_average_scale =
                1 << SI_LOAD_SHIFT;
            float load_average_for_1_minute =
                system.loads[0] / load_average_scale;
            float load_average_for_5_minutes =
                system.loads[1] / load_average_scale;
            float load_average_for_15_minutes =
                system.loads[1] / load_average_scale;

            uint64_t total_ram =
                system.totalram * system.mem_unit;
            uint64_t used_ram =
                (system.totalram - system.freeram) * system.mem_unit;
            uint64_t free_ram =
                system.freeram * system.mem_unit;
            uint64_t total_swap =
                system.totalswap * system.mem_unit;
            uint64_t used_swap =
                (system.totalswap - system.freeswap) * system.mem_unit;
            uint64_t free_swap =
                system.freeswap * system.mem_unit;

            scale_size(
                total_ram,
                total_ram_scaled,
                sizeof(total_ram_scaled)
            );
            scale_size(
                used_ram,
                used_ram_scaled,
                sizeof(used_ram_scaled)
            );
            scale_size(
                free_ram,
                free_ram_scaled,
                sizeof(free_ram_scaled)
            );
            scale_size(
                total_swap,
                total_swap_scaled,
                sizeof(total_swap_scaled)
            );
            scale_size(
                used_swap,
                used_swap_scaled,
                sizeof(used_swap_scaled)
            );
            scale_size(
                free_swap,
                free_swap_scaled,
                sizeof(free_swap_scaled)
            );

            wredrawln(stdscr, 0, header_height);
            mvprintw(
                0, 0,
                "up for %lu %s %lu:%lu:%lu, tasks: %zu\n"
                "load average: %.2f, %.2f, %.2f\n"
                "\n"
                "total ram:  %*s, used ram:  %*s, free ram:  %*s\n"
                "total swap: %*s, used swap: %*s, free swap: %*s\n",
                uptime_days,
                uptime_days == 1 ?
                    "day" : "days",
                uptime_hours,
                uptime_minutes,
                uptime_seconds,
                pid_list_length,
                load_average_for_1_minute,
                load_average_for_5_minutes,
                load_average_for_15_minutes,
                Memory_Column_Width,
                total_ram_scaled,
                Memory_Column_Width,
                used_ram_scaled,
                Memory_Column_Width,
                free_ram_scaled,
                Memory_Column_Width,
                total_swap_scaled,
                Memory_Column_Width,
                used_swap_scaled,
                Memory_Column_Width,
                free_swap_scaled
            );

            werase(pad);

            int real_pad_height = 0;
            for (size_t i = 0; i < pid_list_length; ++i) {
                struct task_info task;
                pid_t pid = pid_list[i];
                if (syscall(__NR_get_task_info, pid, &task) == 0) {
                    if (!show_kernel_threads &&
                            (task.pid  == Kernel_Thread_Daemon_PID ||
                             task.ppid == Kernel_Thread_Daemon_PID)) {
                        continue;
                    }

                    const char *task_state =
                        task.state < Task_States_Count - 1 ?
                            Task_States[task.state + 1] :
                            Task_States[0];

                    scale_time(
                        task.user_cpu_time,
                        user_cpu_time_scaled,
                        sizeof(user_cpu_time_scaled)
                    );
                    scale_time(
                        task.system_cpu_time,
                        kernel_cpu_time_scaled,
                        sizeof(kernel_cpu_time_scaled)
                    );

                    scale_size(
                        task.core_memory_bytes_used,
                        core_memory_usage_scaled,
                        sizeof(core_memory_usage_scaled)
                    );
                    scale_size(
                        task.virtual_memory_bytes_used,
                        virtual_memory_usage_scaled,
                        sizeof(virtual_memory_usage_scaled)
                    );
                    scale_size(
                        task.bytes_read,
                        data_read_scaled,
                        sizeof(data_read_scaled)
                    );
                    scale_size(
                        task.bytes_written,
                        data_written_scaled,
                        sizeof(data_written_scaled)
                    );

                    has_attribute = false;
                    if (has_colors()) {
                        if (task.state == 0 &&
                                task.pid != current_pid) {
                            wattron(pad, COLOR_PAIR(1));
                            has_attribute =
                                !has_attribute;
                        }
                    }

                    wprintw(
                        pad,
                        "%-*d %-*d %-*s %-*d %-*d %-*s %*d "
                        "%*s %*s %*s %*s "
                        "%*"PRIu64" %*"PRIu64" "
                        "%*s %*s\n",
                        PID_Column_Width,     (int) pid,
                        PPID_Column_Width,    task.ppid,
                        Name_Column_Width,    task.name,
                        UID_Column_Width,     task.uid,
                        GID_Column_Width,     task.gid,
                        State_Column_Width,   task_state,
                        Nice_Column_Width,    task.nice_level,
                        UTime_Column_Width,   user_cpu_time_scaled,
                        KTime_Column_Width,   kernel_cpu_time_scaled,
                        RSS_Column_Width,     core_memory_usage_scaled,
                        VM_Column_Width,      virtual_memory_usage_scaled,
                        Reads_Column_Width,   task.read_syscalls_count,
                        Writes_Column_Width,  task.write_syscalls_count,
                        Read_Column_Width,    data_read_scaled,
                        Written_Column_Width, data_written_scaled
                    );

                    if (has_attribute) {
                        wattroff(pad, COLOR_PAIR(1));
                    }

                    ++real_pad_height;
                }
            }

            int window_height, window_width;
            getmaxyx(stdscr, window_height, window_width);

            wnoutrefresh(stdscr);
            prefresh(
                header_pad,
                0,                 pad_shift_x,
                header_height,     0,
                header_height + 1, window_width - 1
            );
            prefresh(
                pad,
                pad_shift_y,       pad_shift_x,
                header_height + 1, 0,
                window_height - 2, window_width - 1
            );
            prefresh(
                footer_pad,
                0,                 0,
                window_height - 1, 0,
                window_height,     window_width - 1
            );
            doupdate();

            int key = wgetch(pad);
            if (key != ERR) {
                switch (key) {
                    case 'h':
                    case KEY_LEFT:
                        --pad_shift_x;
                        break;
                    case 'j':
                    case KEY_DOWN:
                        ++pad_shift_y;
                        break;
                    case 'k':
                    case KEY_UP:
                        --pad_shift_y;
                        break;
                    case 'l':
                    case KEY_RIGHT:
                        ++pad_shift_x;
                        break;
                    case 't':
                        show_kernel_threads =
                            !show_kernel_threads;

                        werase(footer_pad);
                        wprintw(
                            footer_pad,
                            "Press 't' to %s kernel threads. "
                            "Press 'q' to exit.",
                            show_kernel_threads ?
                                "hide" : "show"
                        );

                        break;
                    case 'q':
                        goto cleanup;
                }

                int pad_height_limit =
                    real_pad_height - 2;

                if (pad_shift_y < 0) {
                    pad_shift_y = 0;
                } else if (pad_shift_y >
                               pad_height_limit) {
                    pad_shift_y =
                        pad_height_limit;
                }

                int pad_width_limit =
                    header_length - 5;

                if (pad_shift_x < 0) {
                    pad_shift_x = 0;
                } else if (pad_shift_x >
                               pad_width_limit) {
                    pad_shift_x =
                        pad_width_limit;
                }
            }
        }

cleanup:
        if (header_pad) {
            delwin(header_pad);
        }
        if (pad) {
            delwin(pad);
        }
        if (footer_pad) {
            delwin(footer_pad);
        }

        endwin();

        return exit_status;
}

