cmake_minimum_required(VERSION 2.8.12.2)
  
project(ultrascan)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS "-Wall -g -ggdb3")
    set(CMAKE_C_FLAGS "-Wall -g -ggdb3 -Wno-misleading-indentation")
else()
    set(CMAKE_CXX_FLAGS "-Wall -O3")
    set(CMAKE_C_FLAGS "-Wall -O3 -Wno-misleading-indentation")
endif()


include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

find_path(HS_HEADER_DIR  hs/hs.h)
if( ${HS_HEADER_DIR} STREQUAL "HS_HEADER_DIR-NOTFOUND")
    message(FATAL_ERROR "hs.h NOT found")
else()
    message(STATUS "Found hs.h in ${HS_HEADER_DIR}/hs")
endif()


find_library(HYPERSCAN_LIB hs)
if( ${HYPERSCAN_LIB} STREQUAL "HYPERSCAN_LIB-NOTFOUND")
    message(FATAL_ERROR "hyperscan library NOT found")
else()
    message(STATUS "Found hyperscan library in ${HYPERSCAN_LIB}")
endif()

find_path(PCRE_HEADER_DIR  pcre.h)
if( ${PCRE_HEADER_DIR} STREQUAL "PCRE_HEADER_DIR-NOTFOUND")
    message(FATAL_ERROR "pcre.h NOT found")
else()
    message(STATUS "Found pcre.h in ${PCRE_HEADER_DIR}")
endif()

find_library(PCRE_LIB pcre)
if( ${PCRE_LIB} STREQUAL "PCRE_LIB-NOTFOUND")
    message(FATAL_ERROR "PCRE library NOT found")
else()
    message(STATUS "Found PCRE library in ${PCRE_LIB}")
endif()

find_library(GTEST_LIB gtest)
if( ${GTEST_LIB} STREQUAL "GTEST_LIB-NOTFOUND")
    message(FATAL_ERROR "gtest library NOT found")
else()
    message(STATUS "Found gtest library in ${GTEST_LIB}")
endif()


find_path(GTEST_HEADER_DIR  gtest/gtest.h)
if( ${GTEST_HEADER_DIR} STREQUAL "GTEST_HEADER_DIR-NOTFOUND")
    message(FATAL_ERROR "gtest.h NOT found")
else()
    message(STATUS "Found gtest.h in ${GTEST_HEADER_DIR}/gtest")
endif()

find_package( Threads )


file(GLOB_RECURSE PROJECT_SRC_FILES RELATIVE ${PROJECT_SOURCE_DIR} "db/*.cpp" "core/*.cpp" "util/*.cpp")

message(STATUS "Found source files: ${PROJECT_SRC_FILES}")

configure_file(
        "${PROJECT_SOURCE_DIR}/cmake/version_config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/version_config.cmake"
        @ONLY)

add_custom_command(OUTPUT "BuildInfo" "${CMAKE_CURRENT_BINARY_DIR}/build_info.h"
                   COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/version_config.cmake" 
                   ARGS ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
                   COMMENT "Generating build info header")


add_library(${PROJECT_NAME} SHARED
    ${PROJECT_SRC_FILES}
    "${CMAKE_CURRENT_BINARY_DIR}/build_info.h"
)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${PROJECT_NAME} PROPERTIES C_VISIBILITY_PRESET hidden)
target_compile_definitions(${PROJECT_NAME} PRIVATE ME_COMPILE_LIBRARY)
target_compile_definitions(${PROJECT_NAME} PRIVATE ME_SHARED_LIBRARY)

file(GLOB_RECURSE PROJECT_HEADER_FILES "include/*.h" )
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${PROJECT_HEADER_FILES}" )

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${HS_HEADER_DIR}/hs 
    ${PCRE_HEADER_DIR} 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/include 
    ${PROJECT_SOURCE_DIR}/db 
    ${PROJECT_SOURCE_DIR}/core 
    ${PROJECT_SOURCE_DIR}/util
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${HYPERSCAN_LIB} ${PCRE_LIB})


add_executable(c-api.out ${PROJECT_SOURCE_DIR}/demo/c-api.c)
target_include_directories(c-api.out PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(c-api.out PRIVATE ${PROJECT_NAME})


add_executable(cpp-api.out ${PROJECT_SOURCE_DIR}/demo/cpp-api.cpp)
target_include_directories(cpp-api.out PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(cpp-api.out PRIVATE ${PROJECT_NAME})


add_executable(didi.out ${PROJECT_SOURCE_DIR}/didi/didi.c ${PROJECT_SOURCE_DIR}/didi/cJSON/cJSON.c)
target_include_directories(didi.out PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(didi.out PRIVATE ${PROJECT_NAME} -lm)


add_executable(fieldset_test ${PROJECT_SOURCE_DIR}/tests/fieldset_test.cpp)
target_include_directories(fieldset_test PRIVATE ${PROJECT_SOURCE_DIR}/include ${GTEST_HEADER_DIR})
target_link_libraries(fieldset_test PRIVATE ${PROJECT_NAME} ${GTEST_LIB} ${CMAKE_THREAD_LIBS_INIT})

add_executable(ruleset_test ${PROJECT_SOURCE_DIR}/tests/ruleset_test.cpp)
target_include_directories(ruleset_test PRIVATE ${PROJECT_SOURCE_DIR}/include ${GTEST_HEADER_DIR})
target_link_libraries(ruleset_test PRIVATE ${PROJECT_NAME} ${GTEST_LIB} ${CMAKE_THREAD_LIBS_INIT})


install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)
