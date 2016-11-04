#include <linux/compiler.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/uaccess.h>

SYSCALL_DEFINE2(get_pids, int, pid_list_length, pid_t __user *, output_pid_list)
{
        long result; pid_t pid;
        struct task_struct *task;

        if (pid_list_length < 0) {
                return -EINVAL;
        }

        result = nr_threads;
        if (pid_list_length && output_pid_list) {
                read_lock(&tasklist_lock);
                for_each_process(task) {
                        pid = task_tgid_vnr(task);
                        if (put_user(pid, output_pid_list++)) {
                                result = -EFAULT;
                                break;
                        }

                        if (--pid_list_length <= 0) {
                                break;
                        }
                }
                read_unlock(&tasklist_lock);
        }

        return result;
}

