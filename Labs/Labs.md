Lab Tasks
=========

During labs students will have to practice with various abstractions created by
Unix and Unix-like kernels by writing various system utility applications.

You can use the AUCA server for this work through SSH at 'auca.space'. The login
has the format `<last name>_<first letter of the first name>`.

Create simple clones of the following common Nix programs. Find out their
purpose, their command-line interface, and return values from the system man
pages. You do not have to be thorough implementing support for all the features
from the manuals. Instead, you should only focus on each program's key
functionality.

Though it is mostly discouraged to use platform-specific system calls directly,
in this lab, for educational purposes, it is required to mostly utilize only
them. Minor exceptions can be allowed for cases not directly related to the key
function of the program (e.g., outputting error messages).

## Lab #1, Input and Output

* `true`
* `false`
* `yes`
* `echo`
* `cat`

Use the command `echo $?` in your shell to test the `true` and `false` commands.

## Lab #2, File System

* `ls`
* `pwd`
* `touch`
* `mkdir`
* `mv`
* `rm`
* `cp`

## Lab #3, Processes

* `xargs`
* `parallel`

## Lab #4, Threads

You should use the POSIX Threads library to implement a parallel Odd-even Sort.

* `sort`

## Lab #5, Sockets

You should use the BSD/POSIX sockets to implement a simple echo-like client/server program.

* netcat (`nc`)

You can use the following [C programs](https://github.com/auca/com.463/tree/master/client_server_example) as a template.

## Extra Points

* Simple clone of `less` that only supports scrolling
* Simple clone of `nano` that only supports basic editing

### Documentation

    man yes
    man true
    man false
    man echo
    man cat
    man ls
    man pwd
    man touch
    man mkdir
    man mv
    man cp
    man rm
    man xargs
    man parallel
    man sort
    man nc

    man syscalls

### Links

* [Beej's Guide to C Programming](http://beej.us/guide/bgc)
* [Unix Environment](https://drive.google.com/file/d/0B85z_dQxOMgLNDN3QTFrSmYxZm8/view)

### Books

#### Contemporary

* _C Programming: A Modern Approach, 2nd Edition by K. N. King_

* _Advanced Programming in the UNIX Environment, 3rd Edition by W. Richard
  Stevens, Stephen A. Rago_

* _Unix Network Programming, Volume 1: The Sockets Networking API, 3rd Edition
  by W. Richard Stevens, Bill Fenner, Andrew M. Rudoff_

* _Programming with POSIX Threads by David R. Butenhof_

#### Classic

* _C Programming Language, 2nd Edition by Brian W. Kernighan, Dennis M.
  Ritchie_

* _The Unix Programming Environment by Brian W. Kernighan, Rob Pike_
