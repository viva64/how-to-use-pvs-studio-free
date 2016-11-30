How to use PVS-Studio for FREE?
===============================

  You can use PVS-Studio code analyzer for free, if you add special comments
  to your source code.

  The utility will add comments to the files located in the specified folders
  and subfolders. The comments are added to the beginning of the files with the
  extensions .c, .cc, .cpp, .cp, .cxx, .c++, .cs. You don't have to change header
  files. If you use files with other extensions, you can customize this utility
  for your needs.

  You can find more details about the free version of PVS-Studio
  here: http://www.viva64.com/en/b/0457/
  
Getting up and running
----------------------

  The steps below will take you through cloning your own repository, then compiling and running the utility yourself:

### Windows:

  1. Install Visual Studio 2015. All desktop editions of Visual Studio 2015 can build this utility.
  2. Open How-To-Use-PVS-Studio-FREE.sln and build Release x64.
  3. After compiling finishes, you can run the following command in Command Prompt:

  ```
  How-To-Use-PVS-Studio-FREE.exe --help
  ```

### Linux:

  1. Install Git then clone our repository.
  2. Install GCC 5.4 or later.
  3. Install CMake.
  4. Open your source folder in Terminal and run the following commands:

  ```
  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release ..
  make
  sudo make install
  ```

  5. After compiling and install finishes, you can run the following command in Terminal:

  ```
  how-to-use-pvs-studio-free --help
  ```
