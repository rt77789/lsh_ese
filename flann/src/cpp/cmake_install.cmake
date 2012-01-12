# Install script for directory: /home/rt777789/code/misc/mariusmuja-flann-023fbdb/src/cpp

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FOREACH(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libflann_cpp.so.1.7.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libflann_cpp.so.1.7"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libflann_cpp.so"
      )
    IF(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      FILE(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    ENDIF()
  ENDFOREACH()
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE SHARED_LIBRARY FILES
    "/home/rt777789/code/misc/mariusmuja-flann-023fbdb/temp/lib/libflann_cpp.so.1.7.0"
    "/home/rt777789/code/misc/mariusmuja-flann-023fbdb/temp/lib/libflann_cpp.so.1.7"
    "/home/rt777789/code/misc/mariusmuja-flann-023fbdb/temp/lib/libflann_cpp.so"
    )
  FOREACH(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libflann_cpp.so.1.7.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libflann_cpp.so.1.7"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libflann_cpp.so"
      )
    IF(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      IF(CMAKE_INSTALL_DO_STRIP)
        EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "${file}")
      ENDIF(CMAKE_INSTALL_DO_STRIP)
    ENDIF()
  ENDFOREACH()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE STATIC_LIBRARY FILES "/home/rt777789/code/misc/mariusmuja-flann-023fbdb/temp/lib/libflann_cpp_s.a")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libflann_cpp-gd.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libflann_cpp-gd.so")
    FILE(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libflann_cpp-gd.so"
         RPATH "")
  ENDIF()
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE SHARED_LIBRARY FILES "/home/rt777789/code/misc/mariusmuja-flann-023fbdb/temp/lib/libflann_cpp-gd.so")
  IF(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libflann_cpp-gd.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libflann_cpp-gd.so")
    IF(CMAKE_INSTALL_DO_STRIP)
      EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libflann_cpp-gd.so")
    ENDIF(CMAKE_INSTALL_DO_STRIP)
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE STATIC_LIBRARY FILES "/home/rt777789/code/misc/mariusmuja-flann-023fbdb/temp/lib/libflann_cpp_s-gd.a")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FOREACH(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libflann.so.1.7.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libflann.so.1.7"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libflann.so"
      )
    IF(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      FILE(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    ENDIF()
  ENDFOREACH()
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE SHARED_LIBRARY FILES
    "/home/rt777789/code/misc/mariusmuja-flann-023fbdb/temp/lib/libflann.so.1.7.0"
    "/home/rt777789/code/misc/mariusmuja-flann-023fbdb/temp/lib/libflann.so.1.7"
    "/home/rt777789/code/misc/mariusmuja-flann-023fbdb/temp/lib/libflann.so"
    )
  FOREACH(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libflann.so.1.7.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libflann.so.1.7"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib64/libflann.so"
      )
    IF(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      IF(CMAKE_INSTALL_DO_STRIP)
        EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "${file}")
      ENDIF(CMAKE_INSTALL_DO_STRIP)
    ENDIF()
  ENDFOREACH()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib64" TYPE STATIC_LIBRARY FILES "/home/rt777789/code/misc/mariusmuja-flann-023fbdb/temp/lib/libflann_s.a")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/home/rt777789/code/misc/mariusmuja-flann-023fbdb/src/cpp/flann" FILES_MATCHING REGEX "/[^/]*\\.h$" REGEX "/[^/]*\\.hpp$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

