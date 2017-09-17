project(ReLoEd C CXX)

cmake_minimum_required(VERSION 2.8)

set(pyromaze_VERSION_MAJOR "1")
set(pyromaze_VERSION_MINOR "0")
set(pyromaze_VERSION_PATCH "0")
set(pyromaze_VERSION_EXTRA "")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Compiler flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ffast-math")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DGLAD_DEBUG")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

#--------------------------------------------------------------------
# Add subdirectories
#--------------------------------------------------------------------
add_subdirectory(deps/Silice3D)
include_directories(SYSTEM ${SILICE3D_INCLUDE_DIRS})

# This should be the last subdir / include
include_directories(${PROJECT_SOURCE_DIR}/src/cpp)
add_subdirectory(src)

