cmake_minimum_required(VERSION 2.8)

project(flowc)

set(SOURCES
    container.cpp
    field.cpp
    filter.cpp
    handler.cpp
    logger.cpp
    network.cpp
    parser.cpp
    settings.cpp
    templates.cpp
    )
set(HEADERS
    include/byte_order.h
    include/container.h
    include/defaults.h
    include/field.h
    include/filter.h
    include/handler.h
    include/logger.h
    include/network.h
    include/parser.h
    include/settings.h
    include/swappable_circular_buffer.h
    include/templates.h)
set (CMAKE_CXX_STANDARD 17)
add_executable(${PROJECT_NAME} main.cpp ${SOURCES} ${HEADERS})
add_executable(${PROJECT_NAME}_viewer main_viewer.cpp ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME}_viewer PUBLIC include)

find_package( Boost 1.65.1 COMPONENTS system program_options REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
target_link_libraries( ${PROJECT_NAME} LINK_PUBLIC ${Boost_LIBRARIES} )
target_link_libraries( ${PROJECT_NAME}_viewer LINK_PUBLIC ${Boost_LIBRARIES} )

target_link_libraries (${PROJECT_NAME} stdc++fs)
target_link_libraries (${PROJECT_NAME}_viewer stdc++fs)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME}_viewer ${CMAKE_THREAD_LIBS_INIT})
