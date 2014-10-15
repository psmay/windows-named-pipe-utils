windows-named-pipe-utils
========================

Windows commands for interfacing named pipes with standard I/O streams.

Synopsis
--------

In the following discussion, `produce` is a program that outputs data on
`stdout`, while `consume` is a program that reads data on `stdin`. We'll
demonstrate how to achieve the same result as

    produce | consume > result.txt

using named pipes.

Here are examples I've used for testing purposes. Substitute to taste.

    @ECHO OFF
    REM produce.bat
    REM Runs seq (as installed with MSYS).
    REM Prints a numeric sequence (here, 1 through 20) to stdout.
    C:\MinGW\msys\1.0\bin\seq.exe 20

    @ECHO OFF
    REM consume.bat
    REM Runs wc (as installed with MSYS).
    REM Prints newline/word/byte counts of stdin.
    C:\MinGW\msys\1.0\bin\wc.exe

### `createAndReadPipe`: Get `stdin` from a new inbound named pipe

On one console, do:

    createAndReadPipe MyPipeName | consume > result.txt

Then, on another console, do:

    produce > \\.\pipe\MyPipeName

### `createAndWritePipe`: Put `stdout` to a new outbound named pipe

On one console, do:

    produce | createAndWritePipe MyPipeName

Then, on another console, do:

    consume < \\.\pipe\MyPipeName > result.txt

Notes
-----

*   `createAndReadPipe` and `createAndWritePipe` are pipe *servers*. The
    program that subsequently opens the pipe for write or read,
    respectively, is a *client*.
*   Each server is designed to open two streams—one to read, one to
    write—and continually read from the first and write to the second.
    When an EOF or broken pipe is detected on either stream, the server
    exits normally and the named pipe ceases to exist.
*   The client program is not required to use redirection to connect.
    The pipe path can usually be opened and used as a regular file, so
    another program may open and use it without any extraordinary calls.
    *   The client must not, however, attempt to open the file before it
        exists. The client may run before the server starts, but it
        cannot open the pipe until the server creates it.

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
