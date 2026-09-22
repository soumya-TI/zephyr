set(TARGET fastrts)

add_library(${TARGET} INTERFACE)

target_include_directories(${TARGET} INTERFACE .)

target_link_libraries(${TARGET} INTERFACE CMSISDSP)
