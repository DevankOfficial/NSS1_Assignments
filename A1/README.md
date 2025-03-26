# NSS1 Assignment 1
Devank Singh
2021320

## Introduction
> This folder contains a Makefile and several folders each associated with a function that is listed below:
> - One => This folder contains all the relevant programs in regards to Part 1 of the    assignment (ACLs and Setuid). It contains getacl.cc and setacl.cc.
> - Two => This folder contains all the relevant programs in regards to Part 2 of the    assignment (Simple sudo). It contains two.cc which is the simple_sudo program. The other programs are for fputs, fgets, cd, & create_dir and a sample text file.
> - utils => This folder contains header files and other programs used for this assignment. This also contains extra programs for testing & handling various access control scenarios.

## How to Run the Programs
1. Simply go into assignment1 directory by:
`cd /path/to/assignment1` 
3. To run programs for Part 1(getacl.cc and setacl.cc), simply go to directory One by: `cd One`. Then run the Makefile by:`make` (This will create all executable files needed)
 - Then to run getacl: `./getacl <file_path>`
 - And to run setacl: `./setacl <file_path> <new_acl>`
 - To run checker: `./checker <file_path> <mode>`
4.  To run programs for Part 2(sudo.cc), simply go to directory Two by: `cd Two`. Then run the Makefile by:`make` (This will create all executable files needed)
 - Then to run the simple sudo: `./sudo <command> <arguments>`.

5. By assumption, the extra created programs are aslo enclosed. This directory contains fget_program.cc, fput_program.cc, cd_program.cc, create_dir_program.cc and sample.txt. The getacl & setacl can also be used with these. To investigate the:
 - read permission of a file use: `./fget <file_name> <byte_to_be_read>`
 - write permission of a file use: `./fput <filename> <string_to_write>`
 - execute and read permission of (parent)directory use: `./cd <dir_name>`
 - write permission of (parent)directory use: `./create_dir <dir_name>`
