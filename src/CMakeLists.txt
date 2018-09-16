set (SCREENSHOOT_VERSION_MAJOR 0)
set (SCREENSHOOT_VERSION_MINOR 1)

configure_file(
    "config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

include_directories(${PROJECT_BINARY_DIR})

file(GLOB SOURCES "*.c")
add_executable(screenshoot ${SOURCES} config.h.in)

target_include_directories(screenshoot PUBLIC ${GTK3_INCLUDE_DIRS})
target_compile_options(screenshoot PUBLIC ${GTK3_CFLAGS_OTHER})
target_link_libraries(screenshoot ${GTK3_LIBRARIES})

install(TARGETS screenshoot RUNTIME DESTINATION bin)