DetailIdentifier
================
================
**Overview**
================

A tool for converting scanned design sketches into 3D models

Project uses a variety of libraries, including:
 * Boost 1.55
 * OpenCV 2.4
 * OpenCASCADE 6.7.1 // not yet
 * Tesseract-OCR 3.0.2 // not yet
 * Juce 3.0.8 (only downloaded)

So make sure you have all of them installed when trying to compile DetailIndentifier.

Additionally other libraries are used, but their code is included in repository, so there is no need to have them already installed. These include:
 * PugiXML // not yet
 * Lua // not yet
 * Potrace 1.11
 * Libboard 0.9
 * dxflib 3.3.4
 * Turtle 1.2.6

Please note that this is not yet even version alpha, so may have no needed functionality

=================
**Dependencies**
=================

You may encounter trouble during building one of libraries that detail_identifier depends on (especially on M$ Windows without Cygwin. £inux and Unix system support Makefiles)
In such case please check out dependency_hacks directory - you may find some useful stuff there.

=================
**Installation**
=================

You should have the following environment variables defined (if you don't want to have them as environment variables 
you can pass them directly to CMake script via -DXXX option):
 * BOOST_ROOT
 * TURTLE_ROOT (in case you wish to build tests)
 * JUCE_ROOT

There are no pre-build binaries, only compilation from sources is supported. Assuming you have both OpenCV and Boost properly installed,
following steps should be sufficient:

***Common***

 * mkdir out
 * cd out
 * cmake ..
 
***Windows***
 * msbuild /m DetailIdentifier.sln

***Linux***
 * make -j4
 
***Assumptions***
Now, this is written under assumption that you have all required libraries installed...
You most probably don't. I'm sorry that you need to install so much stuff, but I needed to speed things up 
as much as possible, as I was running out of time. If app will still be developed, we'll try to get rid of most dependencies
as soon as we get some time.

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
