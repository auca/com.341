### COM 341, Operating Systems
# Practice Task #1

![ish](http://i.imgur.com/hUwm2PG.gif)

### Required Tools

Ensure that you have the Bash shell with common Unix utilities (GNU coreutils or
any alternatives), and the Git version control system installed on your system.

* On Windows, you can get the official Git
  [distribution](https://git-scm.com/downloads) with everything you need in one
  package.

* On macOS, you can get Git by installing Xcode or the Command Line Tools
  [package](https://developer.apple.com/opensource) from Apple.

* On Ubuntu Linux, you can install Git through the system's package manager with
  the command `sudo apt install git`.

macOS and Ubuntu Linux have Bash and basic Unix programs installed by default.

### Requirements

The goal of the task is to write code to communicate with the kernel of an
operating system through machine instructions of a particular CPU architecture.
The CPU architecture selected for this year is ARMv8-A.

* [First](https://github.com/auca/com.341/tree/master/Practice%201/debian/arm64),
  you need to prepare an execution environment on you personal computer (with an
  x86 or x86-64 CPU architecture). To emulate the ARMv8-A CPU we will use the
  QEMU hardware emulator. Through QEMU we will install and use a 64-bit ARMv8
  version of the Debian Linux operating system.

* [Second](https://github.com/auca/com.341/tree/master/Practice%201/ish), you
  need to implement a number of system calls in assembly language for the CPU at
  hand for a simple interactive shell.

* At the end, you need to submit your solution to our server from the emulated
  system with Git.

### Deadlines

You will have two weeks from the announcement date to finish your work.
