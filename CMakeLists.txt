cmake_minimum_required(VERSION 3.10)
project(util)

set(CMAKE_VERBOSE_MAKEFILE OFF)

add_library(${PROJECT_NAME} STATIC
    fileio.c
    hash.c
    str.c
    mem.c
    cmd.c
    #ptrlst.c
    except.c
    baselst.c
)

#target_include_directories(${PROJECT_NAME}
#    PUBLIC
#        ${PROJECT_SOURCE_DIR}/../include
#        ${PROJECT_SOURCE_DIR}/../common
#)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -DUSE_GC
        $<$<CONFIG:DEBUG>:-g >
        #$<$<CONFIG:DEBUG>:-Og >
        $<$<CONFIG:DEBUG>:-DMEMORY_DEBUG >
        $<$<CONFIG:DEBUG>:-DENA_TRACE >
        $<$<CONFIG:RELEASE>:-Ofast >
        $<$<CONFIG:PROFILE>:-pg -O0 >
)

add_custom_target(base_test
    COMMENT "Test the basic list functionality"
    COMMAND gcc -Wall -Wextra -Wpedantic -g -DUSE_GC -I.. -L. -o base_test ../base_test.c -lutil -lgc
)

add_custom_target(cmd_test
    COMMENT "Test the command line functionality"
    COMMAND gcc -Wall -Wextra -Wpedantic -g -DUSE_GC -I.. -L. -o cmd_test ../cmd_test.c -lutil -lgc
)

add_custom_target(except_test
    COMMENT "Test the exceptions functionality"
    COMMAND gcc -Wall -Wextra -Wpedantic -g -DUSE_GC -I.. -L. -o except_test ../except_test.c -lutil -lgc
)

add_custom_target(hash_test
    COMMENT "Test the hash table functionality"
    COMMAND gcc -Wall -Wextra -Wpedantic -g -DUSE_GC -I.. -L. -o hash_test ../hash_test.c -lutil -lgc
)

add_custom_target(str_test
    COMMENT "Test the string functionality"
    COMMAND gcc -Wall -Wextra -Wpedantic -g -DUSE_GC -I.. -L. -o str_test ../str_test.c -lutil -lgc
)

add_custom_target(all_tests
    COMMENT "Build all tests"
    COMMAND make base_test && make cmd_test && make except_test && make hash_test && make str_test
)