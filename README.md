How to use PVS-Studio for FREE?
===============================

  You can use PVS-Studio code analyzer for free, if you add special comments
  to your source code.

  The utility will add comments to the files located in the specified folders
  and subfolders. The comments are added to the beginning of the files with the
  extensions .c, .cc, .cpp, .cp, .cxx, .c++, .cs. You don't have to change header
  files. If you use files with other extensions, you can customize this utility
  for your needs.

  You can find more details about the free version of PVS-Studio [here](http://www.viva64.com/en/b/0457/).

Getting up and running
----------------------

  Binaries for Linux and Windows can be found [here](https://github.com/viva64/how-to-use-pvs-studio-free/releases).
  
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

### Note:

There was a large discussion on the topic whether it is permitted to add and remove comments in the source code so that to use the analyzer for free. We consider this method to be unacceptable, as it is a way to bypass the restrictions, imposed on a free license. Therefore, we decided to add the following section to the license agreement:

You have the right to use PVS-Studio for free by adding special comments to the source code of your project. To find out which comments should be added and the way to automate this process, read the article: "How to use PVS-Studio for Free".

Note that this mode is not intended to evaluate this software. Please use a demo version or request a temporary license key to try out the analyzer.

You should insert these comments when you have already decided to use the analyzer on a regular basis. You can remove these comments later, but it is possible to remove them only once.

It is not allowed to add and remove the comments in the source code repeatedly. It is also not allowed to append comments multiple times and subsequently roll back the content of comments in source files to initial state using version control system. Such actions are intended to evade the terms that the source code should contain comments and are a violation of the analyzer usage agreement.
