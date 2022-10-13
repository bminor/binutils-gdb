cmake_minimum_required(VERSION 3.10)
project(cheri_compressed_cap C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
if(APPLE AND NOT (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
    # XXX: ugly hack, would be nice if llvm could find this
    include_directories(SYSTEM /Library/Developer/CommandLineTools/usr/include/c++/v1)
endif()

include(CheckCXXCompilerFlag)
# CMAKE_REQUIRED_LINK_OPTIONS needs cmake 3.13
set(CMAKE_REQUIRED_LIBRARIES -fsanitize=undefined)
check_cxx_compiler_flag(-fsanitize=undefined HAVE_UBSAN)
set(CMAKE_REQUIRED_LIBRARIES -fsanitize=address)
check_cxx_compiler_flag(-fsanitize=address HAVE_ASAN)
set(CMAKE_REQUIRED_LIBRARIES -fsanitize=memory)
check_cxx_compiler_flag(-fsanitize=memory HAVE_MSAN)
# XXX: needs instrumented system libraries
set(HAVE_MSAN FALSE)
set(CMAKE_REQUIRED_LIBRARIES -fsanitize=fuzzer)
check_cxx_compiler_flag(-fsanitize=fuzzer HAVE_LIBFUZZER)

include(CheckCCompilerFlag)
check_c_compiler_flag(-Wunused-but-set-variable HAVE_UNUSED_BUT_SET_VARIABLE)
if (HAVE_UNUSED_BUT_SET_VARIABLE)
    add_compile_options(-DHAVE_UNUSED_BUT_SET_VARIABLE)
endif()

if (HAVE_UBSAN)
    add_compile_options(-fsanitize=undefined)
    if (${CMAKE_VERSION} VERSION_GREATER 3.12)
        add_link_options(-fsanitize=undefined)
    else()
        link_libraries(-fsanitize=undefined)
    endif()
endif()

add_compile_options(-Wno-gnu-statement-expression)
add_compile_options(-fno-common)

add_compile_options(-pedantic -Wall -Wextra -Wno-old-style-cast -Wno-c++98-compat-pedantic -Wno-c++98-c++11-compat-binary-literal -Wno-c11-extensions -Wno-padded)
# TODO: -Weverything
add_library(cheri_compressed_cap14 cheri_compressed_cap.c cheri_compressed_cap.h)

function(add_decompress_cap _format)
    add_executable(decompress_cap_${_format} decompress_cap_${_format}.c)
    include(GNUInstallDirs)
    install(TARGETS decompress_cap_${_format} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endfunction()

# TODO: add_decompress_cap(64)
add_decompress_cap(128)
add_decompress_cap(128m)

# Test against sail generated C code:
# This needs GMP (and CMake doesn't include a find module)
find_package(PkgConfig)
pkg_check_modules(PC_GMP gmp)
find_path(GMP_INCLUDE_DIR gmp.h
          HINTS ${PC_GMP_INCLUDEDIR} ${PC_GMP_INCLUDE_DIRS}
          PATH_SUFFIXES gmp)
message(STATUS "GMP include dir: ${GMP_INCLUDE_DIR}")
find_library(GMP_LIBRARY NAMES gmp
             HINTS ${PC_GMPL_LIBDIR} ${PC_GMP_LIBRARY_DIRS})
message(STATUS "GMP library: ${GMP_LIBRARY}")

if (NOT GMP_LIBRARY)
    message(FATAL_ERROR "Cannot find GMP library")
endif()
if (NOT GMP_INCLUDE_DIR)
    message(FATAL_ERROR "Cannot find GMP includes")
endif()
add_library(GMP::GMP UNKNOWN IMPORTED)
set_target_properties(GMP::GMP PROPERTIES
  IMPORTED_LOCATION "${GMP_LIBRARY}"
  INTERFACE_INCLUDE_DIRECTORIES "${GMP_INCLUDE_DIR}"
)

function(add_sail_wrapper _format _suffix _sanitizer_flags)
    add_library(sail_wrapper_${_format}${_suffix} STATIC test/sail_wrapper_${_format}.c test/sail.c test/rts.c test/sail_failure.c)
    if (_sanitizer_flags)
        target_compile_options(sail_wrapper_${_format}${_suffix} PRIVATE "${_sanitizer_flags}")
        target_link_libraries(sail_wrapper_${_format}${_suffix} PRIVATE "${_sanitizer_flags}")
    endif()
    target_link_libraries(sail_wrapper_${_format}${_suffix} PUBLIC GMP::GMP)  # needs GMP, should propagate to all users
    target_compile_options(sail_wrapper_${_format}${_suffix} PRIVATE -Wno-unused-function -Wno-unused-parameter -Wno-sign-compare -Wno-unused-label -Wno-extra-semi -Wno-gnu-binary-literal)
endfunction()

function(add_sail_wrappers _format)
    add_sail_wrapper(${_format} "" "")
    if (HAVE_ASAN)
        add_sail_wrapper(${_format} "-asan-ubsan" "-fsanitize=undefined,address")
    endif()
    if (HAVE_MSAN)
        add_sail_wrapper(${_format} "-msan-ubsan" "-fsanitize=undefined,memory")
    endif()
endfunction()

add_sail_wrappers(64)
add_sail_wrappers(128)
add_sail_wrappers(128m)

# Add test exes for sail generated code

function(add_test_sail_wrapper _format)
    add_executable(test_sail_wrapper_${_format} decompress_cap_${_format}.c)
    target_compile_definitions(test_sail_wrapper_${_format} PRIVATE DECOMPRESS_WITH_SAIL_GENERATED_CODE=1)
    target_link_libraries(test_sail_wrapper_${_format} PRIVATE sail_wrapper_${_format})
endfunction()

# TODO: add_test_sail_wrapper(64)
add_test_sail_wrapper(128)
add_test_sail_wrapper(128m)

enable_testing()

function(add_cc_test _format _target _suffix _sanitizer_flags)
    add_executable(${_target}_${_format}${_suffix} test/${_target}_${_format}.cpp)
    if (_sanitizer_flags)
        target_compile_options(${_target}_${_format}${_suffix} PRIVATE "${_sanitizer_flags}")
        target_link_libraries(${_target}_${_format}${_suffix} PRIVATE "${_sanitizer_flags}")
    endif()
    if (NOT _format STREQUAL "256")
        target_link_libraries(${_target}_${_format}${_suffix} PRIVATE sail_wrapper_${_format}${_suffix})
    endif()
    add_test(NAME test-${_target}_${_format}${_suffix} COMMAND ${_target}_${_format}${_suffix})
endfunction()

function(add_cc_tests_target _format _target)
    add_cc_test(${_format} ${_target} "" "")
    if (HAVE_ASAN)
        add_cc_test(${_format} ${_target} "-asan-ubsan" "-fsanitize=undefined,address")
    endif()
    if (HAVE_MSAN)
        add_cc_test(${_format} ${_target} "-msan-ubsan" "-fsanitize=undefined,memory")
    endif()
endfunction()

function(add_cc_tests _format)
    add_cc_tests_target(${_format} simple_test)
    if (NOT _format STREQUAL "256")
        # TODO: sail_setbounds_128m not yet implemented
        if (NOT _format STREQUAL "128m")
            add_cc_tests_target(${_format} setbounds_test)
        endif()
        # TODO: 41047 failed assertions
        if (NOT _format STREQUAL "128m")
            add_cc_tests_target(${_format} random_inputs_test)
        endif()
    endif()
endfunction()

add_cc_tests(64)
add_cc_tests(128)
add_cc_tests(128m)
add_cc_tests(256)

function(add_fuzz_tests _format)
    if (HAVE_LIBFUZZER)
        if (HAVE_ASAN)
            add_sail_wrapper(${_format} "-fuzzer-asan-ubsan" "-fsanitize=undefined,address,fuzzer")
            add_executable(fuzz_decompress_${_format}-asan test/fuzz_decompress_${_format}.cpp)
            target_compile_options(fuzz_decompress_${_format}-asan PRIVATE -fsanitize=undefined,address,fuzzer)
            target_link_libraries(fuzz_decompress_${_format}-asan PRIVATE -fsanitize=undefined,address,fuzzer)
            target_link_libraries(fuzz_decompress_${_format}-asan PRIVATE sail_wrapper_${_format}-fuzzer-asan-ubsan)
        endif()
        if (HAVE_MSAN)
            add_sail_wrapper(${_format} "-fuzzer-msan-ubsan" "-fsanitize=undefined,memory,fuzzer")
            add_executable(fuzz_decompress_${_format}-msan test/fuzz_decompress_${_format}.cpp)
            target_compile_options(fuzz_decompress_${_format}-msan PRIVATE -fsanitize=undefined,memory,fuzzer)
            target_link_libraries(fuzz_decompress_${_format}-msan PRIVATE -fsanitize=undefined,memory,fuzzer)
            target_link_libraries(fuzz_decompress_${_format}-msan PRIVATE sail_wrapper_${_format}-fuzzer-msan-ubsan)
        endif()
    endif()
endfunction()

add_fuzz_tests(64)
add_fuzz_tests(128)
add_fuzz_tests(128m)
