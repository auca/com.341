Operating Systems, Practice Tasks
=================================

During labs students will have to practice with various abstractions created by Unix and Unix-like kernels by writing various system utility applications.

Create simple clones of the following common Nix programs. Find out their purpose, their command-line interface, and return values from the system man pages. You do not have to be thorough implementing support for all the features from the manuals. Instead, you should only focus on each program's key functionality.

Though it is mostly discouraged to use platform-specific system calls directly, in this lab, for educational purposes, it is required to mostly utilize only them. Minor exceptions can be allowed for cases not directly related to the key function of the program (e.g., outputting error messages).

## Developer Tools

The following tool is required to be installed to work on this course. It contains the Git version control system that you will use to submit your work to the instructor. The Git installation package also includes a virtual terminal Mintty, the command interpreter Bash, and an SSH client to connect and work with the course server.

* [Git SCM](https://git-scm.com)

On macOS, it is recommended to install the official command-line developer tools from Apple by opening the `Terminal.app` and running the following command.

```bash
xcode-select --install
```

The following code editor is optional. It is not required, but it can be helpful if you will not feel comfortable working with the command-line interface for long periods. You have to install the 'Remote - SSH' extension from the editor to use it with our remote server.

* [Microsoft Visual Studio Code](https://code.visualstudio.com)

This virtualization software and the OS images will allow you to set up your development environment on your computer.

* [Oracle VM VirtualBox](https://www.virtualbox.org) on Windows, GNU/Linux
* [VMware Fusion](https://www.vmware.com/products/desktop-hypervisor/workstation-and-fusion) on macOS

To controll the virtual machines from the command line, you will need the following tools.

* [Vagrant](https://www.vagrantup.com)

Use the following Vagrantfile to create a virtual machine with the Ubuntu 24.04 for lab tasks and most projects.

* [Course Vagrantfile](https://github.com/auca/com.341/blob/master/Other/Vagrantfile)

## Lab #1, Input and Output

* `true`
* `false`
* `echo`
* `yes`
* `cat`

Use the command `echo $?` in your shell to test the `true` and `false` commands.

## Lab #2, File System

* `pwd`
* `touch`
* `mkdir`
* `mv`
* `rm`
* `cp`
* `ls`

## Lab #3, Processes

* `xargs`

## Lab #4, Threads

You should use the POSIX Threads library to implement a parallel odd-even or merge sort.

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
* [Unix Environment](https://drive.google.com/file/d/0B85z_dQxOMgLNDN3QTFrSmYxZm8/view?usp=sharing&resourcekey=0-M_TMmM3wqZuxQF68g5lIVQ)

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
