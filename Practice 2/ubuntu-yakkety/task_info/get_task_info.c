#include <linux/bitops.h>
#include <linux/compiler.h>
#include <linux/cputime.h>
#include <linux/cred.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/pid.h>
#include <linux/pid_namespace.h>
#include <linux/rcupdate.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/user_namespace.h>
#include <task_info/task_info.h>

SYSCALL_DEFINE2(get_task_info, pid_t, pid, struct task_info __user *, output_task_info)
{
        struct user_namespace *user_namespace;
        struct pid_namespace *pid_namespace;

        struct task_struct *task;
        const struct cred *task_credentials;

        unsigned int state;
        cputime_t user_cpu_time,
                  system_cpu_time,
                  user_cpu_time_scaled,
                  system_cpu_time_scaled;

        struct task_info local_task_info;

        if (output_task_info) {
                if (pid < 0) {
                        return -EINVAL;
                }

                /* based on `kernel/tsacct.c` */

                rcu_read_lock();
                pid_namespace = task_active_pid_ns(current);
                task = find_task_by_pid_ns(pid, pid_namespace);
                if (task) {
                        get_task_struct(task);
                        rcu_read_unlock();

                        /* pid */
                        local_task_info.pid = pid;
                        /* name */
                        get_task_comm(local_task_info.name, task);
                        local_task_info.name[TASK_COMM_LEN] = '\0';

                        /* uid, gid, ppid */
                        rcu_read_lock();
                        task_credentials = __task_cred(task);
                        user_namespace = current_user_ns();
                        local_task_info.ppid =
                                pid_alive(task) ?
                                        task_tgid_nr_ns(
                                                rcu_dereference(
                                                        task->real_parent
                                                ),
                                                pid_namespace
                                        ) :
                                        0;
                        local_task_info.uid =
                                from_kuid_munged(
                                        user_namespace,
                                        task_credentials->uid
                                );
                        local_task_info.gid =
                                from_kgid_munged(
                                        user_namespace,
                                        task_credentials->gid
                                );
                        rcu_read_unlock();

                        /* state */
                        state = (task->state | task->exit_state) & TASK_REPORT;
                        if (task->state == TASK_PARKED) {
                                state = TASK_INTERRUPTIBLE;
                        }
                        local_task_info.state = fls(state);
                        /* nice */
                        local_task_info.nice_level = task_nice(task);

                        /* user_cpu_time, system_cpu_time */
                        task_cputime(
                                task,
                                &user_cpu_time,
                                &system_cpu_time
                        );
                        local_task_info.user_cpu_time =
                                cputime_to_usecs(user_cpu_time);
                        local_task_info.system_cpu_time =
                                cputime_to_usecs(system_cpu_time);

                        /* user_cpu_time_scaled, system_cpu_time_scaled */
                        task_cputime_scaled(
                                task,
                                &user_cpu_time_scaled,
                                &system_cpu_time_scaled
                        );
                        local_task_info.user_cpu_time_scaled =
                                cputime_to_usecs(user_cpu_time_scaled);
                        local_task_info.system_cpu_time_scaled =
                                cputime_to_usecs(system_cpu_time_scaled);

                        /* core_memory_bytes_used, virtual_memory_bytes_used */
                        if (likely(task->mm)) {
                                local_task_info.core_memory_bytes_used =
                                        get_mm_rss(task->mm) * PAGE_SIZE;
                                local_task_info.virtual_memory_bytes_used =
                                        task->mm->total_vm * PAGE_SIZE;
                        }

                        /* minor_page_fault_count, major_page_fault_count */
                        local_task_info.minor_page_fault_count =
                                task->min_flt;
                        local_task_info.major_page_fault_count =
                                task->maj_flt;

                        /* bytes_read, bytes_written */
                        local_task_info.bytes_read =
                                task->ioac.rchar;
                        local_task_info.bytes_written =
                                task->ioac.wchar;
                        /* read_syscalls_count, write_syscalls_count */
                        local_task_info.read_syscalls_count =
                                task->ioac.syscr;
                        local_task_info.write_syscalls_count =
                                task->ioac.syscw;

                        put_task_struct(task);

                        if (copy_to_user(
                                output_task_info,
                                &local_task_info,
                                sizeof(struct task_info)
                            )) {
                                return -EFAULT;
                        }
                } else {
                        rcu_read_unlock();
                        return -ESRCH;
                }
        }

        return 0;
}

