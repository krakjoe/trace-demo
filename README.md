Tracing Demo
------------

This is a **demo** application that uses [krakjoe/trace](https://github.com/krakjoe/trace) to identify hot functions, and within those functions hot lines ...

It is very terrible code, and should probably not be deployed ... I've never used curses and suck so bad at ui things ...

Building
--------

    mkdir build
    cd build
    cmake ..
    make

Executing
---------

    ./demo [PID]
    
Notes
-----

The purpose of this repo is to demonstrate how to write applications with libphptrace, this is by no means a complete application.

Please, don't deploy this, or make pull requests, use it only as inspiration to write your own applications with libphptrace.

Watch It
--------

Here is a (crappy, short) [terminal cast](https://asciinema.org/a/SGHICn2KuRrqF5ZB6RsriXYTa)
