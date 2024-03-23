cmake_minimum_required(VERSION 3.24)
set(CMAKE_C_STANDARD 11)

project(asciibanner)

add_executable(banner banner_windows.c text.c)

add_executable(bmpbanner bmpbanner_windows.c libbmp.c)

find_package(Freetype REQUIRED)
target_link_libraries(banner PRIVATE Freetype::Freetype)
target_link_libraries(bmpbanner PRIVATE Freetype::Freetype)

# file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/msyh.ttc DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

IF (WIN32)
    install(TARGETS banner bmpbanner
            RUNTIME_DEPENDENCIES
            PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
            POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
            DESTINATION bin)
ENDIF()
# install(FILES msyh.ttc DESTINATION bin)