cmake_minimum_required(VERSION 2.8)
project(flowc)

set(FLOWCD_SRC
    common.cpp
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

set(FLOWC_SRC
    common.cpp
    container.cpp
    field.cpp
    logger.cpp
    network.cpp
    settings.cpp
)

set(FLOWCD_HDR
    include/byte_order.h
    include/common.h
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
    include/templates.h
)

set(FLOWC_HDR
    include/common.h
    include/container.h
    include/field.h
    include/logger.h
    include/network.h
    include/settings.h
    include/crow.h
)

set (CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}d main_d.cpp ${FLOWCD_SRC} ${FLOWCD_HDR})
add_executable(${PROJECT_NAME} main.cpp ${FLOWC_SRC} ${FLOWC_HDR})
target_include_directories(${PROJECT_NAME}d PUBLIC include)
target_include_directories(${PROJECT_NAME} PUBLIC include)

find_package(Boost COMPONENTS system program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME}d LINK_PUBLIC ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${Boost_LIBRARIES})

target_link_libraries (${PROJECT_NAME}d LINK_PUBLIC stdc++fs)
target_link_libraries (${PROJECT_NAME} LINK_PUBLIC stdc++fs)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME}d LINK_PUBLIC ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${CMAKE_THREAD_LIBS_INIT})
