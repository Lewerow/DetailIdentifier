DetailIdentifier
================
================
**Overview**
================

A tool for converting scanned design sketches into 3D models

Project uses a variety of libraries, including:
 * Boost 1.55
 * OpenCV 2.4
 * OpenCASCADE 6.7.1
 * Tesseract-OCR 3.0.2

So make sure you have all of them installed when trying to compile DetailIndentifier.

Additionally other libraries are used, but their code is included in repository, so there is no need to have them already installed. These include:
 * PugiXML
 * Lua

Please note that this is not yet even version alpha, so may have no needed functionality

=================
**Installation**
=================

There are no pre-build binaries, only compilation from sources is supported.

=================
**Building**
=================

A C++11 compatible compiler is required - currently supported compilers are:
 * Clang-3.5
 * GCC-4.9
 * CL-18.0 (MSVS 2013)

MinGW build is not tested, so might not work.
In case you wish to use this application and encounter problems during build, please contact development team.

Project uses CMake (version 2.8 is minimal supported) as its buildsystem, so it should work everywhere where CMake is supported and all required libraries are available.
