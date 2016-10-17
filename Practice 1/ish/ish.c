#ifdef ISH_USE_STDLIB
    #include <unistd.h>
    #include <stdlib.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <sys/wait.h>
    #include <sys/types.h>
#endif

#include "ish_cstring_utilities.h"
#include "ish_shell_utilities.h"
#include "ish_syscalls.h"

/*
    The maximum length of the input string
*/
static const unsigned long Max_Input_String_Length = 255;
/*
    The maximum number of program arguments
*/
static const unsigned long Max_Argument_Count = 255;
/*
    The maximum string length of the executable path
*/
static const unsigned long Max_Executable_Path_Length = 1024;

/*
    The 'HOME' environment variable that contains the path
    to the home directory of the current user
*/
static const char Environment_Variable_Home[] = "HOME";
/*
    The 'PATH' environment variable that contains a list
    of directories with executable files to search for
*/
static const char Environment_Variable_Path[] = "PATH";

/*
    The shell builtin command string to change the working directory
*/
static const char Builtin_Command_CD[] = "cd";
/*
    The shell builtin command string to exit from the shell
*/
static const char Builtin_Command_Exit[] = "exit";

/*
    The standard input file flags

        O_RDONLY
*/
static const unsigned short Standard_Input_File_Flags = 00000;
/*
    The file mode for a file created for a redirection of the standard output
    stream

        user        | group  | other
        read, write   read     read
         4  +  2       4        4
*/
static const unsigned short Standard_Output_File_Mode = 00644;

/*
    Error messages
*/
static const char Fork_Error_Message[] = "fork failure\n";

/*
    ish
    An insecure silent and primitive shell
*/
int main(int argc, char **argv, char **envp)
{
    /*
        Extract the home directory path for the current user.
    */
    char *home =
        ish_get_first_environment_variable(
            Environment_Variable_Home,
            sizeof(Environment_Variable_Home),
            envp
        );

    /*
        Extract the list of directories where executable programs are located.
    */
    char *paths =
        ish_get_first_environment_variable(
            Environment_Variable_Path,
            sizeof(Environment_Variable_Path),
            envp
        );

    char input[Max_Input_String_Length + 1];
    input[Max_Input_String_Length] = '\0';

    /*
        `read` tries to write a fixed number of bytes into a
        buffer from a specific file descriptor.

        We read an input string on each iteration
        from the 'standard input' stream.

        `man 2 read`
    */
    while (read(0, input, Max_Input_String_Length) >= 0) {
        /*
            Mark the 'new line' character as the end of the C string.
        */
        ish_replace_first_character_in_cstring(
            input, '\n', '\0'
        );

        char *arguments[Max_Argument_Count + 1];
        arguments[Max_Argument_Count] = 0;

        /*
            Build a null-terminated array of arguments from the input string.
        */
        unsigned long argument_count =
            ish_build_argument_array_from_input(
                input,
                arguments,
                Max_Argument_Count
            );

        if (arguments[0]) {
            const char *command =
                arguments[0];

            /*
                Process builtin commands that can not be implemented as a
                separate process.
            */
            if (ish_are_cstrings_equal(
                    command,
                    Builtin_Command_CD
                )) {
                char *directory =
                    argument_count == 0 ?
                        home : arguments[1];

                if (directory) {
                    /*
                        `chdir` changes the current working directory (CWD)
                        for the calling process to the specified path. All
                        child processes will inherit the new working
                        directory from the parent.

                        `man 2 chdir`
                    */
                    chdir(directory);
                }
            } else if (ish_are_cstrings_equal(
                           command,
                           Builtin_Command_Exit
                       )) {
                int exit_status =
                    argument_count == 0 ?
                        0 : ish_get_integer_from_cstring(
                                arguments[1]
                            );

                /*
                    `exit` asks the system to unload the process with a
                    provided status code. The status code can be received,
                    analyzed, and processed by a parent process
                    (see `waitpid`).

                    `man 2 exit`
                */
                exit(exit_status);
            } else {
                /*
                    Start a separate process.
                */

                char *executable =
                    arguments[0];

                /*
                    Check whether the specified executable exists or search
                    for it through directories in the 'PATH' environment
                    variable.
                */

                char candidate[Max_Executable_Path_Length + 1];
                candidate[Max_Executable_Path_Length] = '\0';

                /*
                    `stat` obtains information about a file pointed to by
                    path. Upon successful completion a value of 0 is returned.
                    Otherwise, a value of -1 is returned (e.g., the file does
                    not exist).

                    `man 2 stat`
                */
                #ifdef ISH_USE_STDLIB
                    struct stat stat_buffer;
                    struct stat *stat_result =
                        &stat_buffer;
                #else
                    char stat_result[256];
                #endif
                if (stat(executable, stat_result) != 0) {
                    executable = 0;
                    for (char *cursor = paths; cursor && !executable; ) {
                        char *path =
                            ish_get_token_in_cstring(
                                cursor,
                                ish_is_path_separator,
                                &cursor
                            );

                        if (path) {
                            ish_combine_path_elements(
                                path,
                                arguments[0],
                                candidate,
                                Max_Executable_Path_Length
                            );

                            if (stat(candidate, stat_result) == 0) {
                                executable =
                                    candidate;
                            }
                        }
                    }
                }

                if (executable) {
                    /*
                        Search for the stdin redirection lexeme in the list
                        of arguments.
                    */
                    char *stdin_file =
                        ish_extract_last_stdin_redirection_file(
                            arguments
                        );

                    /*
                        If the stdin redirection lexeme was consumed and an
                        input file path was provided, try to open the file
                        and get the input file descriptor that will be used
                        in place of stdin.

                        The `open` system call opens or creates a file for
                        reading or writing (specified by the combination of
                        flags). The call returns -1 on error or 0 on success.

                        `man 2 open`
                    */
                    int stdin_descriptor =
                        !stdin_file ?
                            -1 : open(
                                     stdin_file,
                                     Standard_Input_File_Flags
                                 );

                    /*
                        Search for the stdout redirection lexeme in the list
                        of arguments.
                    */
                    char *stdout_file =
                        ish_extract_last_stdout_redirection_file(
                            arguments
                        );

                    /*
                        If the stdout redirection lexeme was consumed and an
                        output file path was provided, try to open the file
                        and get the output file descriptor that will be used
                        in place of stdout.

                        The `creat` system call creates a file for writing.
                        Unix file permissions are specified as a second
                        parameter. The call returns -1 on error or 0 on
                        success.

                        `man 2 open`

                        http://www.tldp.org/LDP/
                            GNU-Linux-Tools-Summary/html/x9543.htm
                    */
                    int stdout_descriptor =
                        !stdout_file ?
                            -1 : creat(
                                     stdout_file,
                                     Standard_Output_File_Mode
                                 );

                    /*
                        `fork` creates a child duplicate of the current process.

                        `man 2 fork`
                    */
                    int pid = fork(); // ~ typedef int pid_t
                    if (pid == 0) {
                        /*
                            A Process ID (PID) of zero from the `fork` call
                            means we're in the child process.
                        */

                        if (stdin_descriptor > 0) {
                            /*
                                `dup2` duplicates an existing file descriptor
                                onto a specified second file descriptor (which
                                will get closed first before the operation).

                                We replace the stdin descriptor 0 with
                                the new input file descriptor. Thus
                                we redirect the standard input stream.

                                `man 2 dup2`
                            */
                            dup2(stdin_descriptor, 0);

                            /*
                                `close` deletes a descriptor.

                                Two file descriptors (`stdin_descriptor` and 0)
                                point to the same kernel I/O data structures.
                                We need to close the first as we won't use it
                                anymore.

                                `man 2 close`
                            */
                            close(stdin_descriptor);
                        }

                        if (stdout_descriptor > 1) {
                            /*
                                We replace the stdout descriptor 1 with
                                the new output file descriptor. Thus
                                we redirect the standard output stream.
                            */
                            dup2(stdout_descriptor, 1);

                            /*
                                Two file descriptors (`stdout_descriptor` and 1)
                                point to the same kernel I/O data structures.
                                We need to close the first as we won't use it
                                anymore.
                            */
                            close(stdout_descriptor);
                        }

                        /*
                            `execve` substitutes the current process image with
                            the provided executable file.

                            The first parameter to `execve` is a file name with
                            the path to the executable. The next is an array of
                            arguments to the new program. The first argument
                            should be the file name of the executable (with or
                            without the path).

                            Array of environment variables such as `HOME` and
                            `PATH` goes as the last parameter.

                            The new process image will get the current working
                            directory and both (if any) redirected standard
                            input and output streams.

                            Execution of the new process image will start from
                            the default executable entry point (e.g., `start`
                            -> `main`).

                            `man 2 execve`
                        */
                        execve(executable, arguments, envp);

                        /*
                            `exit` here is only reached on unsuccessful
                            `execve`.
                        */
                        exit(-1);
                    } else if (pid > 0) {
                        /*
                            A positive Process ID (PID) from the `fork` call
                            means we are in the parent process.
                        */

                        /*
                            `waitpid` waits for a child process to change its
                            state (stopped, exited, terminated...).

                            The status output parameter can contain an exit
                            code or a terminating signal after the call.

                            `man 2 waitpid`
                        */
                        int child_status;
                        waitpid(pid, &child_status, 0);
                    } else {
                        /*
                            A negative value from the `fork` call means that
                            something went wrong and the child process was
                            not created.
                        */

                        /*
                            `write` tries to write a fixed number of bytes from
                            the buffer into the location specified by the file
                            descriptor.

                            We output an error message for every failed `fork`
                            system call into the 'standard error' stream.

                            `man 2 write`
                        */
                        write(2, Fork_Error_Message, sizeof(Fork_Error_Message));
                    }
                }
            }
        }
    }

    return 0;
}
