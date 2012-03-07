YAPTCC
======

YAPT Commandline Client written in C with libcurl.
Makefile template taken from suckless.org.

Usage
-----

Upload public:
   ./yaptcc < file

Upload private:
   ./yaptcc -e password < file

Download public:
   ./yaptcc id > file

Download private:
   ./yaptcc -e password > file
