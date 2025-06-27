cmake_minimum_required(VERSION 3.14)
project(file_transfer C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra)

# for headers 
include_directories(src)

# for signals
add_compile_definitions(_POSIX_C_SOURCE=200809L)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message(FATAL_ERROR "Error: the project is Linux only")
endif()

file(GLOB_RECURSE COMMON_SRC ${CMAKE_SOURCE_DIR}/src/common/*.c)
file(GLOB_RECURSE CLIENT_SRC ${CMAKE_SOURCE_DIR}/src/client/*.c)
file(GLOB_RECURSE SERVER_SRC ${CMAKE_SOURCE_DIR}/src/server/*.c)

# remove the main functions so tests can use these
list(REMOVE_ITEM CLIENT_SRC ${CMAKE_SOURCE_DIR}/src/client/client.c)
list(REMOVE_ITEM SERVER_SRC ${CMAKE_SOURCE_DIR}/src/server/server.c)

set(CLIENT_MAIN ${CMAKE_SOURCE_DIR}/src/client/client.c)
set(SERVER_MAIN ${CMAKE_SOURCE_DIR}/src/server/server.c)

# --- Client ---
add_executable(client 
  ${CLIENT_MAIN}
  ${CLIENT_SRC} 
  ${COMMON_SRC}
)

# --- Server ---
add_executable(server 
  ${SERVER_MAIN}
  ${SERVER_SRC} 
  ${COMMON_SRC}
)

# --- Test ---
enable_testing()
file(GLOB TESTS tests/*.c)

set(TEST_TARGETS "")

foreach(test_src ${TESTS})
    get_filename_component(test_name ${test_src} NAME_WE)
    
    add_executable(${test_name} 
      ${test_src} 
      ${COMMON_SRC} 
      ${CLIENT_SRC} 
      ${SERVER_SRC}
    )
    
    set_target_properties(${test_name} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/.test_bins
      ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
      LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
      OBJECT_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/tests
    )
    
    add_test(NAME ${test_name} COMMAND ${test_name})

    list(APPEND TEST_TARGETS ${test_name})
endforeach()

add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS ${TEST_TARGETS}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Compiling and running all tests..."
)
