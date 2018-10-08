#include "ish_syscalls.h"

/*
    Function Calling Convention

    On x86 (Linux, BSD, Mac OS X) arguments are passed on the stack and pushed
    from right to left.

        buffer_size is pushed first
        the buffer pointer is pushed next
        file_descriptor is pushed last

    Result is returned in the eax register.

    On x86-64 (Linux, BSD, Mac OS X) the first six integer or pointer
    arguments are passed in registers rdi, rsi, rdx, rcx, r8, and r9.

       file_descriptor goes to rdi
       the buffer pointer goes to rsi
       buffer_size goes to rdx

    Result is returned in the rax register.

    On ARM (A32) (Linux, BSD, Mac OS X) the first four integer or pointer
    arguments are passed in registers r0 through r3. The same registers are
    used to return values.

       file_descriptor goes to r0
       the buffer pointer goes to r1
       buffer_size goes to r2

    Result is returned in the r0 register.

    On ARM (A64) (Linux, BSD, Mac OS X) the first eight integer or pointer
    arguments are passed in registers x0 through x7. The same registers are
    used to return values.

       file_descriptor goes to x0
       the buffer pointer goes to x1
       buffer_size goes to x2

    Result is returned in the x0 register.

    ---
    Kernel Calling Convention

    Refer to slides from Practice #2.
*/
long ish_read(
        int file_descriptor,
        void *buffer,
        unsigned long buffer_size
     )
{
    /*
        // AT&T/UNIX GCC Inline Assembly Sample

        static const char Argument[] =                   // C constants
            "some data";
        static const unsigned long Another_Argument =
            sizeof(Argument);

        long result;                                     // a C variable

        // x86, x86-64

        __asm__ __volatile__ (
            "op-code<length suffix> %%src_register, %%dest_register\n\t"
            "op-code<length suffix> $immediate, %%dest_register\n\t"
            // ...
            "op-code<length suffix> %<argument number>, %%dest_register\n\t"
            "op-code"
            : "=a" (result)                              // output argument/s
            : "D" ((unsigned long) file_descriptor),     // input arguments
              "S" (buffer),
              "d" (buffer_size),
              "r" (Argument), "r" (Another_Argument)
            : "%used register", "%another used register" // clobbered registers
        );

        // The ARM assembly syntax uses the `#` symbol for constants and NOT
        // the `$` symbol. Registers `r` or `x` (for the 32-bit or 64-bit
        // architecture) do not need a `%%` prefix.

        // `__asm__` and `__volatile__` could also be written as
        // `asm`     and `volatile`.

        // The `volatile` modifier tells the compiler not to remove or reorder
        // the inlined assembly block during the compiler optimization step.

        // <length suffixes>
        //     'b'    'w'     's'     'l'     'q'
        //      8 bit  16 bit  16 bit  32 bit  64 bit  integers
        //                     32 bit  64 bit          floating point numbers
        //
        // Length suffixes are not required for the ARM assembly syntax.

        // Argument numbers go from top to bottom, from left to right
        // starting from zero.
        //
        //     result           : %<argument number> = %0
        //     file_descriptor  : ...                = %1
        //     buffer           :                    = %2
        //     buffer_size      :                    = %3
        //     Argument         :                    = %4
        //     Another_Argument :                    = %5

        // The first quoted letter before the argument in brackets is a
        // register constraint. It tells the compiler to provide the
        // argument through that register.
        //
        // On X86/-64 the following register constraints are possible
        // +---+--------------------------+
        // | r :   any register           |
        // +---+--------------------------+
        // | a :   %rax, %eax, %ax, %al   |
        // | b :   %rbx, %ebx, %bx, %bl   |
        // | c :   %rcx, %ecx, %cx, %cl   |
        // | d :   %rdx, %edx, %dx, %dl   |
        // | S :   %rsi, %esi, %si        |
        // | D :   %rdi, %edi, %di        |
        // +---+--------------------------+
        //
        // On ARM, the `r` constraint will work for all general purpose
        // registers. The input variable's register can be specified after the
        // variable's declaration wrapped in quotes and parentheses.
        //
        //     register long result ("r7"); // 32-bit ARM
        //     register long result ("x0"); // 64-bit ARM
        //
        // All registers used as input or output arguments should not be
        // listed as clobbered.
        //
        // https://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html
    */

#if defined(__i386__) || defined(__x86_64__)
    #if defined(__APPLE__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #elif defined(__linux__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #endif
#elif defined(__arm__) || defined(__aarch64__)
    #if defined(__APPLE__)
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #elif defined(__linux__)
        #if defined(__aarch64__)
            register long result asm("x0");                                      

            __asm__ __volatile__ (                                               
                "mov x8, #0x3F\n\t"                                              
                "svc #0x0"                                                       
                : "=r" (result)                                                  
                : : "x8"                                                         
            );                                                                   

            return result;
        #elif defined(__arm__)
            register long result asm("r0");

            __asm__ __volatile__ (
                "mov r7, #0x3\n\t"
                "swi #0x0"
                : "=r" (result)
                : : "r7"
            );

            return result;
        #endif
    #endif
#else
    return -1;
#endif
}

int ish_chdir(const char *path)
{
#if defined(__i386__) || defined(__x86_64__)
    #if defined(__APPLE__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #elif defined(__linux__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #endif
#elif defined(__arm__) || defined(__aarch64__)
    #if defined(__APPLE__)
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #elif defined(__linux__)
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #endif
#else
    return -1;
#endif
}

void ish_exit(int status)
{
#if defined(__i386__) || defined(__x86_64__)
    #if defined(__APPLE__)
        #if defined(__x86_64__)

        #elif defined(__i386__)

        #endif
    #elif defined(__linux__)
        #if defined(__x86_64__)

        #elif defined(__i386__)

        #endif
    #endif
#elif defined(__arm__) || defined(__aarch64__)
    #if defined(__APPLE__)
        #if defined(__aarch64__)

        #elif defined(__arm__)

        #endif
    #elif defined(__linux__)
        #if defined(__aarch64__)

        #elif defined(__arm__)

        #endif
    #endif
#endif
}

int ish_stat(const char *path, void *stat_result)
{
#if defined(__i386__) || defined(__x86_64__)
    #if defined(__APPLE__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #elif defined(__linux__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #endif
#elif defined(__arm__) || defined(__aarch64__)
    #if defined(__APPLE__)
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #elif defined(__linux__)
        /*
            `stat` doesn't work on aarch64 on Linux. The call should be
            implemented through `fstatat` with the first parameter set to
            AT_FDCWD (-100) to simulate the work of `stat`.
        */
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #endif
#else
    return -1;
#endif
}

int ish_open(const char *path, int flags)
{
#if defined(__i386__) || defined(__x86_64__)
    #if defined(__APPLE__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #elif defined(__linux__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #endif
#elif defined(__arm__) || defined(__aarch64__)
    #if defined(__APPLE__)
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #elif defined(__linux__)
        /*
            `open` doesn't work on aarch64 on Linux. The call should be
            implemented through `openat` with the first parameter set to
            AT_FDCWD (-100) to simulate the work of `open`.
        */
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #endif
#else
    return -1;
#endif
}

int ish_creat(const char *path, unsigned int mode)
{
#if defined(__i386__) || defined(__x86_64__)
    #if defined(__APPLE__)
        /*
            On Apple systems you should implement `creat` with
            `open`.

            int open(user_addr_t path, int flags, int mode)...
                where int flags

                    O_CREAT | O_TRUNC | O_WRONLY
                    0x0200  | 0x0400  | 0x0001 = 0x601
        */
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #elif defined(__linux__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #endif
#elif defined(__arm__) || defined(__aarch64__)
    #if defined(__APPLE__)
        /*
            On Apple systems you should implement `creat` with
            `open`.

            int open(user_addr_t path, int flags, int mode)...
                where int flags

                    O_CREAT | O_TRUNC | O_WRONLY
                    0x0200  | 0x0400  | 0x0001 = 0x601
        */
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #elif defined(__linux__)
        /*
            `creat` doesn't work on aarch64 on Linux. The call should be
            implemented through `openat` with the first parameter set to
            AT_FDCWD (-100) to simulate the work of `creat`.
        */
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #endif
#else
    return -1;
#endif
}

int ish_dup2(int old_file_descriptor, int new_file_descriptor)
{
#if defined(__i386__) || defined(__x86_64__)
    #if defined(__APPLE__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #elif defined(__linux__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #endif
#elif defined(__arm__) || defined(__aarch64__)
    #if defined(__APPLE__)
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #elif defined(__linux__)
        /*
            `dup2` doesn't work on aarch64 on Linux. The call should be
            implemented through `dup3` with the last parameter set to zero to
            simulate the work of `dup2`.
        */
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #endif
#else
    return -1;
#endif
}

int ish_close(int file_descriptor)
{
#if defined(__i386__) || defined(__x86_64__)
    #if defined(__APPLE__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #elif defined(__linux__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #endif
#elif defined(__arm__) || defined(__aarch64__)
    #if defined(__APPLE__)
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #elif defined(__linux__)
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #endif
#else
    return -1;
#endif
}

int ish_fork()
{
#if defined(__i386__) || defined(__x86_64__)
    #if defined(__APPLE__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #elif defined(__linux__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #endif
#elif defined(__arm__) || defined(__aarch64__)
    #if defined(__APPLE__)
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #elif defined(__linux__)
        /*
            `fork` doesn't work on aarch64 on Linux. The call should be
            implemented through `clone` with all parameters set to zero and the
            flags parameter set to SIGCHLD (17) to simulate the work of `fork`.
        */
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #endif
#else
    return -1;
#endif
}

int ish_execve(
        const char *path,
        char *const arguments[],
        char *const environment[]
    )
{
#if defined(__i386__) || defined(__x86_64__)
    #if defined(__APPLE__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #elif defined(__linux__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #endif
#elif defined(__arm__) || defined(__aarch64__)
    #if defined(__APPLE__)
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #elif defined(__linux__)
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #endif
#else
    return -1;
#endif
}

int ish_waitpid(int pid, int *status, int options)
{
#if defined(__i386__) || defined(__x86_64__)
    #if defined(__APPLE__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #elif defined(__linux__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #endif
#elif defined(__arm__) || defined(__aarch64__)
    #if defined(__APPLE__)
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #elif defined(__linux__)
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #endif
#else
    return -1;
#endif
}

long ish_write(
        int file_descriptor,
        const void *buffer,
        unsigned long buffer_size
     )
{
#if defined(__i386__) || defined(__x86_64__)
    #if defined(__APPLE__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #elif defined(__linux__)
        #if defined(__x86_64__)

            return -1;
        #elif defined(__i386__)

            return -1;
        #endif
    #endif
#elif defined(__arm__) || defined(__aarch64__)
    #if defined(__APPLE__)
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #elif defined(__linux__)
        #if defined(__aarch64__)

            return -1;
        #elif defined(__arm__)

            return -1;
        #endif
    #endif
#else
    return -1;
#endif
}
