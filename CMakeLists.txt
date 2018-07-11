###########################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2017
# European Synchrotron Radiation Facility
# CS40220 38043 Grenoble Cedex 9
# FRANCE
#
# Contact: lima@esrf.fr
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################

cmake_minimum_required(VERSION 3.1)

project (mycamera)
include(cmake/project_version.cmake)

# Check if processlib is being used directly or via add_subdirectory
set(PROCESSLIB_MASTER_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(PROCESSLIB_MASTER_PROJECT ON)
endif()

if (NOT PROCESSLIB_MASTER_PROJECT)
  find_package(Lima REQUIRED)
endif()

# Enable python binding code compilation using sip generator
option(PROCESSLIB_ENABLE_PYTHON "compile python binding code?" LIMA_ENABLE_PYTHON)

if (CAMERA_ENABLE_PYTHON)
  find_package(PythonInterp)
  find_package(PythonLibs)
  if(${PYTHONINTERP_FOUND})
    # python site-packages folder
    execute_process(
      COMMAND ${PYTHON_EXECUTABLE} -c "from distutils.sysconfig import get_python_lib; print (get_python_lib())"
      OUTPUT_VARIABLE _PYTHON_SITE_PACKAGES_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
  endif()
  find_package(SIP)
  include(SIPMacros)

  set(PYTHON_SITE_PACKAGES_DIR ${_PYTHON_SITE_PACKAGES_DIR} CACHE PATH "where should python modules be installed?")

  find_package(NumPy)
  if(${NUMPY_FOUND})
    include_directories(${NUMPY_INCLUDE_DIRS})
    link_directories(${NUMPY_LIBRARIES})
  else()
      message(FATAL_ERROR "NumPy not found, cannot build python extensions")
  endif()
  if(WIN32)
    set(SIP_TAGS WIN32_PLATFORM)
  elseif(UNIX)
    set(SIP_TAGS POSIX_PLATFORM)
  endif(WIN32)
  set(SIP_EXTRA_OPTIONS -e -g)
endif()

# Library definition
add_library(mycamera SHARED
  src/camera.cpp
  src/interface.cpp)

target_link_libraries(mycamera ${SDK_LIBRARIES})
target_include_directories(mycamera PUBLIC ${SDK_INCLUDE_DIRS})
target_include_directories(mycamera PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(WIN32)
  target_compile_definitions(mycamera PRIVATE LIBMYCAMERA_EXPORTS)
  set_target_properties(mycamera PROPERTIES PREFIX "lib")
endif()

# Binding code for python
if(PROCESSLIB_ENABLE_PYTHON)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sip/mycamera.sip sip/mycamera.sip)
  set(SIP_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/sip")
  add_sip_python_module(mycamera "${CMAKE_CURRENT_BINARY_DIR}/sip/mycamera.sip")
  target_include_directories(python_module_mycamera PRIVATE ${PYTHON_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/sip)
  target_link_libraries(python_module_mycamera mycamera)
endif()

# Install
include(GNUInstallDirs)
install(
  TARGETS mycamera
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT library)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.h")
