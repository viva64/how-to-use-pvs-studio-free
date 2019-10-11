# 2006-2008 (c) Viva64.com Team
# 2008-2019 (c) OOO "Program Verification Systems"
cmake_minimum_required(VERSION 3.10)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.14)
set(CMAKE_CXX_STANDARD 17)

project(How-To-Use-PVS-Studio-FREE CXX)

set(SOURCES
    main.cpp
    comments.cpp
    encoding.cpp
    )

option(PVS_STUDIO_SHARED "Use shared libraries" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else ()
    if (NOT APPLE)
        set(CMAKE_INSTALL_PREFIX "/usr")
    endif ()

    if (NOT PVS_STUDIO_SHARED AND NOT APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
        set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
        set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
    endif ()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wold-style-cast")

    link_libraries(stdc++fs)
endif ()

if (NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    string(TOLOWER "${PROJECT_NAME}" NAME)
else ()
    set(NAME "${PROJECT_NAME}")
endif ()

add_executable("${NAME}" ${SOURCES})
install(TARGETS "${NAME}" DESTINATION bin)

