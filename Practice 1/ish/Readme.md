### COM 341, Operating Systems
# Practice Task #1, System Calls

### Initial Steps

1. Move into the directory with the Debian hard drive image
   `./Practice 1/debian/arm64`.

        cd 'Practice 1/debian/arm64'

2. On Windows, ensure the path to the QEMU executable is in the $PATH
   environment variable.

        echo $PATH

        # If the QEMU directory is not there, from `debian/arm64/` execute
        export PATH=$PATH:`cd ../../qemu; pwd`

3. Start the 64-bit ARMv8 Debian Linux in QEMU.

        ./start.sh

4. Start a new shell instance.

5. Copy the `ish` directory from your machine to the emulated system with `scp`.
   The target directory should have the architecture name `arm64` or `amd64` at
   the end of its name.

        scp -r ish user@127.0.0.1:~/ish_<arch>

6. Log in into the Debian system through SSH.

        ssh -p 2222 user@127.0.0.1

7. Go into the newly created directory `task_01`.

        cd ish_<arch>

   In this task, you need to perform a number of system calls to the Linux
   kernel on the 64-bit ARMv8 CPU architecture.

   The calls are

        read
        chdir
        exit
        stat
        open
        creat
        dup2
        close
        fork # for extra points
        execve
        waitpid
        write

   You have sources of a simple shell that performs the following calls with the
   the help of a system library.

   Your task is to implement functions

        ish_read
        ish_chdir
        ish_exit
        ish_stat
        ish_open
        ish_creat
        ish_dup2
        ish_close
        ish_fork # for extra points
        ish_execve
        ish_waitpid
        ish_write

   in `ish_syscalls.c` to perform the system calls to the kernel directly on
   your own. That will remove the dependency on the system library for the shell
   at hand.

   Do not forget to add prototypes for your custom functions to
   `ish_syscalls.h`. Open documentation files for each related system call to
   get information about return values and parameter types.

        man 2 read
        man 2 write
        ...

   Library type definitions can be replaced with the following

        typedef size_t  unsigned long
        typedef ssize_t long
        typedef pid_t   int

   The `struct stat` from `sys/stat.h` for the `stat` system call can be
   replaced with an array large enough to contain file information from the
   kernel (as we are only interested in the return code). For an invalid pointer
   to a `struct stat`, the `stat` system call always returns an error.

        char stat[512];

   Test each custom system call function by replacing a related standard library
   call in `ish.c` with a call to your new implementation. When everything is
   ready and works as expected, return the old system call names in `ish.c` and
   remove the `-D ISH_USE_STDLIB` preprocessor definition from the project's
   Makefile. Your shell will use the new calls after that. You can always switch
   back by adding the the definition back.

   You can stop depending on the C standard library by putting the following
   compiler flags `-nostartfiles -nodefaultlibs -nostdlib -static` at the end of
   `CFLAGS=...`.

### Managing Sources and Executables

* Edit the code. Add prototypes and implementation for every system call in
  files `ish_syscall.h` and `ish_syscall.c`. Change related system call names in
  `ish.c` to the newly implemented function names in `ish_syscall.h` (e.g.,
  change `read` to `ish_read`).

        nano ish_syscalls.h
        nano ish_syscalls.c
        nano ish.c

* Build or rebuild the shell.

        make # to compile the native system
             # version of the application (32 for 32-bit OS, 64 for 64-bit OS)

* Test the shell by typing builtin commands such as `cd` and `exit`. Try to
  start various system programs such as `ls` or `date` with an absolute path to
  the executable or without it, with parameters or without them. Try to redirect
  standard input or output streams with `<` and `>` operators.

        ./ish
        /bin/ls
        /bin/ls -l /etc

  press `CTRL+C` to exit

        ./ish
        cd
        ls
        exit

        ./ish
        date
        exit

        ./ish
        cd /tmp
        ls > listing.txt
        cat < listing.txt
        exit 1

* Remove compiled files.

        make clean

### Submitting Work

1. Create a git repository.

        git init
        git config --global user.name <Full Name>
        git config --global user.email <E-mail>

1. Create a new snapshot of your work with an appropriate description.

        git commit -am "Implement system calls <system call name>, ..."

2. Connect to the instructor's remote and submit your work.

        git remote add origin <TBD: the remote will be announced in class>
        git push origin master

### Resources

* [GCC Documentation, How to Use Inline Assembly Language in C Code](https://gcc.gnu.org/onlinedocs/gcc/Using-Assembly-Language-with-C.html)

* [GCC-Inline-Assembly-HOWTO](https://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html)

* [ARM GCC Inline Assembler Cookbook](http://www.ethernut.de/en/documents/arm-inline-asm.html)

* [Linux x86-64 System Call Tables](https://github.com/torvalds/linux/tree/master/arch/x86/entry/syscalls/syscall_64.tbl)

* [Linux x86 System Call Tables](https://github.com/torvalds/linux/tree/master/arch/x86/entry/syscalls/syscall_32.tbl)

* [Bionic Standard C Library, Linux ARMv7-A System Call Table](https://github.com/aosp-mirror/platform_bionic/blob/master/libc/kernel/uapi/asm-arm/asm/unistd-common.h)

* [Bionic Standard C Library, Linux ARMv8-A System Call Table](https://github.com/android/platform_bionic/blob/master/libc/kernel/uapi/asm-generic/unistd.h)

* [Bionic Standard C Library, x86-64 System Calls](https://github.com/android/platform_bionic/tree/master/libc/arch-x86_64/syscalls)

* [Bionic Standard C Library, x86 System Calls](https://github.com/android/platform_bionic/tree/master/libc/arch-x86/syscalls)

* [Bionic Standard C Library, ARMv7-A System Calls](https://github.com/android/platform_bionic/tree/master/libc/arch-arm/syscalls)

* [Bionic Standard C Library, ARMv8-A System Calls](https://github.com/android/platform_bionic/tree/master/libc/arch-arm64/syscalls)

### Documentation

    man 2 syscall
    man 2 syscalls

    man 2 read
    man 2 chdir
    man 2 exit
    ...

### Reading

* [C Books and Guides](https://github.com/auca/com.341/blob/master/Labs.md#links)

* _Understanding the Linux kernel, Third Edition by Daniel P. Bovet and Marco Cesati, Chapters 4, 10_

* _Linux Kernel Development, Third Edition by Robert Love, Chapters 5, 7_

