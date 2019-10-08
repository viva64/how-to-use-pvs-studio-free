How to use PVS-Studio for FREE? [![Build Status](https://travis-ci.org/viva64/how-to-use-pvs-studio-free.svg?branch=master)](https://travis-ci.org/viva64/how-to-use-pvs-studio-free)  [![Docs](https://img.shields.io/readthedocs/pip.svg)](https://www.viva64.com/en/b/0457/) ![Platforms](https://img.shields.io/badge/platform-linux%20|%20windows%20|%20macos-green)
===============================

  You can use PVS-Studio code analyzer for free, if you add special comments
  to your source code.

  The utility will add comments to the files located in the specified folders
  and subfolders. The comments are added to the beginning of the files with the
  extensions .c, .cc, .cpp, .cp, .cxx, .c++, .cs, .java. You don't have to change header
  files. If you use files with other extensions, you can customize this utility
  for your needs.

  You can find more details about the free version of PVS-Studio [here](https://www.viva64.com/en/b/0457/).

Getting up and running
----------------------

  Binaries for Linux and Windows can be found [here](https://github.com/viva64/how-to-use-pvs-studio-free/releases).
  
  The steps below will take you through cloning your own repository, then compiling and running the utility yourself:

### Windows:

  1. Install Visual Studio 2017 or higher. All desktop editions of Visual Studio can build this utility;
  2. Install CMake (10.3 or higher);
  3. Open your source folder in Developer Command Prompt and run the following commands:
  
  ```
  mkdir build;
  cd build;
  cmake -G "Visual Studio 15 2017 Win64" ..;
  cmake --build . --config Release;
  ```

### Linux/macOS:

  1. Install GCC (8 or higher).
  2. Install CMake (10.3 or higher).
  3. Open your source folder in Terminal and run the following commands:

  ```
  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release ..
  make

  ```

### Note:

There was a large discussion on the topic whether it is permitted to add and remove comments in the source code so that to use the analyzer for free. We consider this method to be unacceptable, as it is a way to bypass the restrictions, imposed on a free license. Therefore, we decided to add the following section to the license agreement:

You have the right to use PVS-Studio for free by adding special comments to the source code of your project. To find out which comments should be added and the way to automate this process, read the article: "How to use PVS-Studio for Free".

Note that this mode is not intended to evaluate this software. Please use a demo version or request a temporary license key to try out the analyzer.

You should insert these comments when you have already decided to use the analyzer on a regular basis. You can remove these comments later, but it is possible to remove them only once.

It is not allowed to add and remove the comments in the source code repeatedly. It is also not allowed to append comments multiple times and subsequently roll back the content of comments in source files to initial state using version control system. Such actions are intended to evade the terms that the source code should contain comments and are a violation of the analyzer usage agreement.
