cmake_minimum_required(VERSION 3.21)
set(CMAKE_C_STANDARD 11)

project(asciibanner)

if (WIN32)
    if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/source-charset:utf-8 /execution-charset:gbk)
    else ()
        add_compile_options(-finput-charset=utf-8 -fexec-charset=gbk)
    endif()
endif()

add_executable(banner banner_windows.c)
add_executable(bmpbanner banner_windows.c libbmp.c)
add_executable(hzk16ascii hzk16banner.c)
add_executable(hzk16bmp hzk16banner.c libbmp.c)

target_compile_definitions(bmpbanner PRIVATE WRITE_BMP)
target_compile_definitions(hzk16bmp PRIVATE WRITE_BMP)
find_package(Freetype REQUIRED)
target_link_libraries(banner PRIVATE Freetype::Freetype)
target_link_libraries(bmpbanner PRIVATE Freetype::Freetype)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/HZK16 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if (WIN32)
    if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/source-charset:utf-8 /execution-charset:gbk)
    else ()
        add_compile_options(-finput-charset=utf-8 -fexec-charset=gbk)
    endif()
    install(TARGETS banner bmpbanner
            RUNTIME_DEPENDENCIES
            PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
            POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
            DESTINATION bin)
endif()
install(TARGETS hzk16ascii hzk16bmp DESTINATION bin)
install(FILES HZK16 DESTINATION bin)