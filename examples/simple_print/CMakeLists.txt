# Set the example files directories
set(EXAMPLE_HEADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/headers")
set(EXAMPLE_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sources")

# Add the headers directories
include_directories(
    ${EXAMPLE_HEADERS_DIR}
)

# Add the source files
file(GLOB_RECURSE EXAMPLE_SOURCES CONFIGURE_DEPENDS
    "${EXAMPLE_SOURCES_DIR}/*.c"
)

# Create the example executable
add_executable(simple_print ${EXAMPLE_SOURCES})

target_link_libraries(simple_print PRIVATE ${PROJECT_NAME})