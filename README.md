windows-named-pipe-utils
========================

Windows commands for interfacing named pipes with stdio streams.

Synopsis
--------

### `createAndReadPipe`: Get stdin from a new named pipe

*   Substitute your receiving program for `md5sum > out.tmp`.
    *   Redirecting `stdout` like this is optional, but
        `createAndReadPipe` prints its own status messages on stderr,
        which can mix with the program's output in unfashionable ways.
*   Substitute your sending program for `seq 20`.
    *   The sending program need not start after the pipe is open nor
        terminate when it is closed, but it must not attempt to open the
        pipe before it is created.
    *   Use of `stdout` is optional. The pipe path can be opened for
        write just like a regular file in most situations (such as with
        `fopen()` or `CreateFile()`, or by way of redirection like in
        this example).

On one console:

    rem Create pipe and read to another program
    createAndReadPipe MyPipeName | md5sum > out.tmp

On another console:

    rem Use another program to write to (and close) the pipe
    rem The pipe path is `\\.\pipe\` followed by the name passed to
    rem `createAndReadPipe`. Input to the pipe is received on `stdin` by the
    rem receiving program.
    seq 20 > \\.\pipe\MyPipeName

Build
-----

This has been successfully built on [MinGW and MSYS](http://mingw.org/)
with GCC 4.8.1. The resulting binary does not depend on DLLs not already
distributed with Windows.

License
-------

Copyright © 2014 Peter S. May

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
