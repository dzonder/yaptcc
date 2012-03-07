YAPTCC
======

YAPT Commandline Client written in C with libcurl.
Makefile template taken from suckless.org.

Installation
------------

To compile without installation::

   $ make

To install (``root`` privileges required)::

   $ make install

Usage
-----

Upload public::

   $ yaptcc < file

Upload private::

   $ yaptcc -e password < file

Download public::

   $ yaptcc id > file

Download private::

   $ yaptcc -e password > file

Requirements
------------

* *nix
* cc
* libcurl

:author: Michal Olech
:version: 0.1
:license: MIT
