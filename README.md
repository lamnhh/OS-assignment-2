# HCMUS - Operating System - Assignment #2

## Problem statement

The assignment contains two parts:

1. A module for generating random numbers. This module must create a character device that processes from user space can open and read random numbers from.

2. A module for hooking to SYS_open and SYS_write. The hook for SYS_open should print which process opens which file, the hook for SYS_write should print which process writes how many bytes and to which file.

## System requirements

The modules are not guaranteed to be working on any operating system other than Ubuntu 16.04 at kernel version 4.15.0-66-generic.

## License

GPL