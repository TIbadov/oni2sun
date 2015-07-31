project(oni2sun)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

set(OPENNI2_INCLUDE_DIR "/home/timur/Lexion/OpenNI-Linux-x64-2.2/Include")
find_library( OPENNI2_LIBRARY
             NAMES OpenNI2
             HINTS ${PC_OPENNI2_LIBDIR} ${PC_OPENNI2_LIBRARY_DIRS} /usr/lib "${OPENNI2_INCLUDE_DIR}/.."
             PATHS "$ENV{PROGRAMFILES}/OpenNI2/Lib${OPENNI2_SUFFIX}"    "$ENV{PROGRAMW6432}/OpenNI2/Lib${OPENNI2_SUFFIX}" "$ENV{PROGRAMW6432}/OpenNI2"
             PATH_SUFFIXES lib lib64
)

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)

include_directories(${OPENNI2_INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${OPENNI2_LIBRARY} ${OpenCV_LIBS} ${Boost_LIBRARIES})

file(COPY intrinsics_example.txt DESTINATION ${CMAKE_BINARY_DIR})
