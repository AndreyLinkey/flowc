cmake_minimum_required(VERSION 2.8)

project(flowc)

find_package( Boost 1.65.1 COMPONENTS system REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries( ${PROJECT_NAME} LINK_PUBLIC ${Boost_LIBRARIES} )

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
