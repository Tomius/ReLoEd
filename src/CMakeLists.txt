cmake_minimum_required(VERSION 2.8)

link_libraries(Silice3D)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DUSE_DEBUG_CONTEXT -g")

if (BUILD_SHARED_LIBS)
    link_libraries("${MATH_LIBRARY}")
endif()

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if (USE_IMAGEMAGICK)
  find_package(ImageMagick COMPONENTS Magick++)
  include_directories(${ImageMagick_INCLUDE_DIRS})
  link_libraries(${ImageMagick_LIBRARIES})
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_IMAGEMAGICK")
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DUSE_IMAGEMAGICK")
else()
  set (LODEPNG_SOURCE "../deps/lodepng/lodepng.cpp")
endif()

file(GLOB PROJECT_SOURCE "cpp/*.cpp" "cpp/*/*.cpp" "cpp/*/*/*.cpp" ${LODEPNG_SOURCE})

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set (PROJECT_BINARY_NAME "ReLoEdd")
else()
    set (PROJECT_BINARY_NAME "ReLoEd")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOGLWRAP_DEBUG=0")
endif()

add_executable(${PROJECT_BINARY_NAME} WIN32 ${PROJECT_SOURCE} ${ICON})

target_include_directories(${PROJECT_BINARY_NAME} PRIVATE)
target_link_libraries(${PROJECT_BINARY_NAME})
set(WINDOWS_BINARIES ${PROJECT_BINARY_NAME})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

if (MSVC)
    # Tell MSVC to use main instead of WinMain for Windows subsystem executables
    set_target_properties(${WINDOWS_BINARIES} PROPERTIES
                          LINK_FLAGS "/ENTRY:mainCRTStartup")
endif()

if (APPLE)
    set_target_properties(empty PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Empty Event")
    set_target_properties(sharing PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Sharing")
    set_target_properties(tearing PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Tearing")
    set_target_properties(threads PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Threads")
    set_target_properties(title PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Title")
    set_target_properties(windows PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Windows")

    set_target_properties(${WINDOWS_BINARIES} PROPERTIES
                          MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
                          MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION_FULL}
                          MACOSX_BUNDLE_INFO_PLIST "${PROJECT_SOURCE_DIR}/deps/glfw/CMake/MacOSXBundleInfo.plist.in")
endif()

