### COM 341, Operating Systems
# Practice Task #2

![Compiling a Linux Kernel](http://i.imgur.com/t0c7Wav.gif)

### General Information

In this task you need to add implementation of two system calls to the Linux
kernel. This will allow a small task information utility to run in the user
space querying information directly from the kernel without parsing output from
the `proc` file system.

### Notes

Consider working in a terminal multiplexer such as `tmux`. In tmux you can
disconnect from the machine during a long-running process, and go back to check
the progress of the task at any moment in time later.

Install a terminal multiplexer

    sudo apt install tmux

Type the following to start a new virtual terminal on your remote server

    tmux

To split the current pane into two vertically

    <CTRL+B> "

To split the current pane into two horizontally

    <CTRL+B> %

To switch to the pane above, below, to the left, or to the right of the current
pane

    <CTRL+B><Up>
    <CTRL+B><Down>
    <CTRL+B><Left>
    <CTRL+B><Right>

To detach from a virtual terminal

    <CTRL+B><D>

To attach back

    tmux a

To exit from a virtual terminal

    exit

### Steps

1. Ensure that your CPU supports `VT-x` or `AMD-V` virtualization extensions.
   Ensure that these extensions are enabled in your BIOS settings. Refer to your
   machine documentation.

2. Download and install [Oracle VirtualBox](https://goo.gl/sIjvXQ), a
   virtualizer for x86 hardware.

3. Download an installation image for [Ubuntu 16.10](http://old-releases.ubuntu.com/releases/16.10).

4. Open Oracle VirtualBox and open a new machine creation dialog by clicking on
   the 'New' button on the toolbar.

5. Type a descriptive name, set type to `Linux` and version to `Ubuntu
   (64-bit)`.

6. Set `memory size` to 1024 megabytes or more. Click on `Create`.

7. Set `file size` for a new virtual disk to 30 gigabytes or more. Once again,
   click on `Create`.

8. Right click on your new machine and select `Settings...`.

9. Go to the `System` tab, select `processor`. Set the first slider to the
   number of cores that you have on your system.

10. Switch to the `Storage` tab, select an empty disk under the IDE controller.
    On the right side, click on the disk icon and select `Choose Virtual Optical
    Disk File...`. Open the disk image from step #2.

11. Go to the `Network` tab, click on `Advanced`, open the `Port Forwarding`
    window. Click on the plus icon and add the following rule to forward traffic
    from your host machine on port 2222 to an SSH server on the guest machine on
    port 22.

        Name      Protocol    Host IP      Host Port    Guest IP     Guest Port
        Rule 1    TCP         127.0.0.1    2222         10.0.2.15    22

    Other options are possible but they depend on your network setup (e.g., you
    can select the `Bridged Adapter` in the `Attached to` drop-down as an
    alternative).

12. Save your settings and use the `Start` button on the toolbar to start your
    virtual machine.

13. On the installer's boot menu, select `Install` to start the installation
    process.

14. On the language selection dialog, set the system language to `English`.

15. Select your location to set up your time zone.

16. Set locale to `United States - en_US.UTF-8`.

17. Select `No` on the keyboard detection dialog and manually select `English
    (US)`.

18. Set a system host name to identify the virtual machine on your network. You
    can use the default value.

19. Type your full name, select a login name, create a password for a new
    account on your virtual system.

20. Select an option that you don't want to encrypt your home directory.

21. Ensure that your time zone was configured properly.

22. Select `Guided - use entire disk` during a disk partitioning step.

23. Select your virtual disk and confirm destructive operations.

24. Set proxy parameters. Ignore the dialog if you don't have a proxy server.

25. Select the option to install security updates automatically.

26. On a software selection dialog, check `OpenSSH server`.

27. Confirm the boot loader installation.

28. When asked, remove the disk image by clicking on a disk icon in the lower
    right corner and selecting `Remove disk from virtual drive`. Reboot the
    virtual machine.

29. You can work directly in the VirtualBox window, or you can connect to your
    server through the SSH protocol.

    On your host machine (from Bash on Windows or any terminal on *nix
    platforms) use the following command to connect to your virtual server.

        ssh -p 2222 <the user name specified during installation>@127.0.0.1

    Consider starting using `tmux` at this point.

30. Uncomment every line that starts with `deb-src` except the line that ends
    with the word `partner` in the package manager's configuration file
    `/etc/apt/sources.list` and save it.

        sudo nano /etc/apt/sources.list

31. Update package index and install updates to the system

        sudo apt update && sudo apt upgrade

32. Install `git`, `make`, `gcc`, and `ncurses-dev`.

        sudo apt install git make gcc ncurses-dev

33. Upload the `Practice 2` directory to the virtual machine.

        scp -P 2222 -r 'Practice 2' <the user name specified during installation>@127.0.0.1:~/

34. Go to the directory with sources of the process information utility
    "tasks".

        cd '~/Practice 2/tasks'

35. Compile the user space program.

        make

36. Ensure that it doesn't work.

        ./tasks

37. Go to your home directory.

        cd ~/

38. Set your name and e-mail address in Git.

        git config --global user.name "your full name"
        git config --global user.email "your e-mail address"

39. Clone the Linux kernel repository for Ubuntu 16.10. Note that Git will try
    to fetch around 200 megabytes of data from the Canonical servers (the company
    behind the Ubuntu OS).

        git clone --depth 1 git://kernel.ubuntu.com/ubuntu/ubuntu-yakkety.git

40. Install prerequisites for building the kernel.

        sudo apt build-dep linux-image-$(uname -r)
        sudo apt install fakeroot

41. Go inside the kernel source tree.

        cd ubuntu-*

42. Create and switch to a new branch to contain all your changes.

        git checkout -b task_info

43. Copy system call entry points into the kernel source tree.

        cp -R ~/Practice\ 2/ubuntu-*/task_info ~/ubuntu-*/

44. Study implementation of two system calls in `task_info/get_pids.c` and
    `task_info/get_task_info.c`. Take a look at the structure passed between
    the kernel and user space in `task_info/include/task_info.h`.

        nano ~/ubuntu-*/task_info/get_pids.c
        nano ~/ubuntu-*/task_info/get_task_info.c
        nano ~/ubuntu-*/task_info/include/task_info/task_info.h

    The `task_info` directory contains implementation of two system calls named
    `get_pids` and `get_task_info`. The first one returns a list of process IDs
    for all running tasks. The second returns task information for a specified
    process ID.

45. Take a look at the structure used to represent threads and processes in the
    Linux kernel. You can find it near the line 1377 at `include/linux/sched.h`
    under the name `task_struct`.

        nano ~/ubuntu-*/include/linux/sched.h

    Check fields the `struct task_struct` contains.

46. Study build rules for the new system calls in `task_info/Makefile`.

        nano ~/ubuntu-*/task_info/Makefile

47. Add a kernel configuration entry to disable or enable the new subsystem.

        nano ~/ubuntu-*/init/Kconfig

    Add the following

        config TASK_INFO
            bool "task_info syscalls"
            default y
            help
              This option enables the task_info system calls which can be
              used to query task information directly from the kernel.

    at the end of the file. Adjust tabs and spaces.

48. Add a forward declaration for the `task_info` structure and two function
    prototypes for `get_pids` and `get_task_info` at the end of
    `include/linux/syscalls.h`.

        nano ~/ubuntu-*/include/linux/syscalls.h

    Add

        struct task_info;

    after

        struct sigaltstack;

    Add the following

        /* task_info */
        asmlinkage long sys_get_pids(int pid_list_length, pid_t __user *output_pid_list);
        asmlinkage long sys_get_task_info(pid_t pid, struct task_info __user *output_task_info);

    at the end of the file before the closing `#endif`

49. Add generic table entries for two system calls before the first instance of
    `#undef __NR_syscalls` in `include/uapi/asm-generic/unistd.h`. Do not forget
    to increment the counter for the first `#define __NR_syscalls` by two.

        nano ~/ubuntu-*/include/uapi/asm-generic/unistd.h

    Add

        /* task_info/get_pids.c */
        #define __NR_get_pids 288
        __SYSCALL(__NR_get_pids, sys_get_pids)
        /* task_info/get_task_info.c */
        #define __NR_get_task_info 289
        __SYSCALL(__NR_get_task_info, sys_get_task_info)

    before

        #undef __NR_syscalls

    Don't forget to adjust numbers `288` and `289` if necessary. Use successive
    values after the last system call in the file.

    Increment the value of the next instance of __NR_syscalls by two.

    For example, change

        #define __NR_syscalls 288

    to

        #define __NR_syscalls 290

50. Add x86 table entries for two system calls at the end of the
    `arch/x86/entry/syscalls/syscall_32.tbl` file.

        nano ~/ubuntu-*/arch/x86/entry/syscalls/syscall_32.tbl

    Add the following

        380    i386    get_pids             sys_get_pids
        381    i386    get_task_info        sys_get_task_info

    at the end of the file. Adjust tabs and spaces.

    Don't forget to adjust numbers `380` and `381` if necessary. Use successive
    values after the last system call in the file.

51. Add x86-64 table entries for two system calls in
    `arch/x86/entry/syscalls/syscall_64.tbl`. Do it before the
    `# x32-specific system call num...` comment.

        nano ~/ubuntu-*/arch/x86/entry/syscalls/syscall_64.tbl

    Add system call numbers for x86-64 architecture

        329    common    get_pids             sys_get_pids
        330    common    get_task_info        sys_get_task_info

    before

        #
        # x32-specific system call numbers start at 512 to avoid cache impact
        # for native 64-bit operation.
        #

    Don't forget to adjust numbers `329` and `330` if necessary. Use successive
    values after the last system call in the file. Adjust tabs and spaces
    appropriately.

52. Add two fallback stubs at the end of `kernel/sys_ni.c`.

        nano ~/ubuntu-*/kernel/sys_ni.c

    Add the following

        /* task_info */
        cond_syscall(sys_get_pids);
        cond_syscall(sys_get_task_info);

    at the end of the file

53. Modify the kernel build system to compile the new calls. Add the
    directory `task_info/` at the end of the line
    `core-y += kernel/ mm/ fs/ ipc/ security/ crypto/ block/` in the main
    `Makefile`.

        nano ~/ubuntu-*/Makefile

    Change the line

        core-y += kernel/ certs/ mm/ fs/ ipc/ security/ crypto/ block/

    to

        core-y += kernel/ certs/ mm/ fs/ ipc/ security/ crypto/ block/ task_info/

54. Add a string `+test` after the version number at the top of
    `debian.master/changelog` to identify your new kernel.

        nano ~/ubuntu-*/debian.master/changelog

    Change the version at the top (the version can be different)

        linux (4.8.0-27.29) yakkety; urgency=low

    to

        linux (4.8.0-27.29+test) yakkety; urgency=low

55. Ensure that you are in the root directory of the kernel source tree.

        cd ~/ubuntu-*

56. Make Debian build scripts executable.

        chmod a+x debian/scripts/*
        chmod a+x debian/scripts/misc/*

57. Clean the build directory.

        fakeroot debian/rules clean

58. Create default configuration files for the kernel.

        fakeroot debian/rules editconfigs

    For each prompt, ensure that the `task_info` option is selected, save the
    configuration and exit.

59. Start the kernel build process. A successful build will produce multiple
    `.deb` packages in the parent directory. Time the the process.

        time fakeroot debian/rules binary-perarch \
                                   binary-indep   \
                                   binary-headers \
                                   binary-generic

    The compilation process can take a lot of time. Compiled objects can use up
    to 10 gigabytes of disk space. The Debian build system not only builds the
    kernel but also packs everything into a set of installable `.deb` packages.
    All compilation errors will appear at this stage.
    
    Put information about the total compilation time into a file
    `<last name>_<first letter of the first name>-time.txt` in the root of
    the Linux source directory.

    To restart an unsuccessful build, fix problems in your sources and start the
    build system again. Add a note to `<last name>_<first letter of the first name>-time.txt`
    that the timing is for a partial recompilation.

        time fakeroot debian/rules binary-perarch \
                                   binary-indep   \
                                   binary-headers \
                                   binary-generic

    To recompile the kernel with new changes after a successful build, remove a
    stamp file to notify the build system that kernel sources were changed.
    After that, you can start the build process again.

        rm debian/stamps/stamp-build-generic

        time fakeroot debian/rules binary-perarch \
                                   binary-indep   \
                                   binary-headers \
                                   binary-generic

60. Go to the parent directory.

        cd ..

61. Ensure that you have a number of newly created `.deb` packages.

        ls *.deb

62. Install the new kernel and all its supporting files.

        sudo dpkg -i *.deb

    In case of dependency errors, run the following

        sudo apt -f install

    and restart the installation process

        sudo dpkg -i *.deb

63. Reboot the machine to start using the new kernel.

        sudo shutdown -r now

64. Reconnect to your machine.

        ssh -p 2222 <the user name specified during installation>@127.0.0.1

65. Go to the directory with sources of the process information utility "tasks".

        cd '~/Practice 2/tasks'

66. Adjust system call numbers that you have defined in the kernel. Change
    values for constants `__NR_get_pids` and `__NR_get_task_info` in `tasks.c`.

        nano tasks.c

67. Recompile the user space program.

        make clean
        make

68. Test the new system calls.

        ./tasks

    You should see a list of tasks that are currently running on the system.
    The program uses your new system calls to get information directly from the
    kernel without going through a `proc` virtual file system (which is how
    programs such as `ps` or `top` work). You can scroll through the list with
    the arrow keys, show or hide kernel threads with the `t` key, or exit by
    pressing `q`.

69. Study the sources of the user space program.

70. Go back to the kernel source tree.

        cd ~/ubuntu-*

71. Create a file listing of the home directory.

        ls ../ > <last name>-<first letter of the first name>-home.txt

72. Create a new repository with all the modified files. Don't forget
    to put a proper name for the `<last name>-<first letter of the first name>-time.txt`
    and `<last name>-<first letter of the first name>-home.txt` files.

        rm -rf .git
        git init
        git add task_info/                             \
                init/Kconfig                           \
                include/linux/syscalls.h               \
                include/uapi/asm-generic/unistd.h      \
                arch/x86/entry/syscalls/syscall_32.tbl \
                arch/x86/entry/syscalls/syscall_64.tbl \
                kernel/sys_ni.c                        \
                Makefile                               \
                debian.master/changelog
        git add <last name>-<first letter of the first name>-time.txt
        git add <last name>-<first letter of the first name>-home.txt

        git commit -m "Add a 'task_info' subsystem"

### Submitting Work

You will get a git remote link to push your modified Linux sources.
Push the changes before the deadline.

### Deadlines

You will have two weeks from the announcement date to finish your work.

### Resources

* [Linux Cross Reference](http://lxr.free-electrons.com)

* [Linux Documentation, Adding a New System Call](https://github.com/torvalds/linux/blob/6f0d349d922ba44e4348a17a78ea51b7135965b1/Documentation/process/adding-syscalls.rst)

* [Linux Filesytem Hierarchy, /proc](http://www.tldp.org/LDP/Linux-Filesystem-Hierarchy/html/proc.html)

### Documentation

    man tmux

    man 2 syscall
    man 2 syscalls
    man 2 getpid
    man 2 sysinfo

### Reading

* _Understanding the Linux kernel, Third Edition by Daniel P. Bovet and Marco
  Cesati, Chapters 3, 4, 7, 10_

* _Linux Kernel Development, Third Edition by Robert Love, Chapters 3-5, 7_
