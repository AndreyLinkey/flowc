cmake_minimum_required(VERSION 2.8)

project(flowc)

set(SOURCES
    main.cpp
    templates.cpp
    field.cpp
    parser.cpp)
set(HEADERS
    include/field.h
    include/templates.h
    include/parser.h
    include/sqlite.h
    include/sqlite_orm.h
    include/swappable_circular_buffer.h)
set (CMAKE_CXX_STANDARD 17)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC include)

find_package( Boost 1.65.1 COMPONENTS system REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
target_link_libraries( ${PROJECT_NAME} LINK_PUBLIC ${Boost_LIBRARIES} )

target_link_libraries (${PROJECT_NAME} sqlite3)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
