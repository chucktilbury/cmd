
project(util)

add_library(${PROJECT_NAME} STATIC
    fileio.c
    hash.c
    str.c
    mem.c
    cmd.c
    ptrlst.c
    except.c
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
