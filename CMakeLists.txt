cmake_minimum_required(VERSION 3.5)

project(ltcstreamer C)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/libltc)

add_executable(ltcstreamer
    ${CMAKE_CURRENT_LIST_DIR}/src/ltcstreamer.c
)

target_link_libraries(ltcstreamer PRIVATE ltc)

install(TARGETS ltcstreamer DESTINATION bin)