cmake_minimum_required(VERSION 3.16.3)

# Project 
project(
    CcompilerbyC
    VERSION 0.1
    DESCRIPTION "c++ simple compiler"
    LANGUAGES CXX)

add_compile_options(-g -Wall -Werror -std=c++2a )

# 라이브러리 파일은 build 디렉토리 안에 lib폴더에 출력.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 실행 파일은 build 디렉토리 안에 bin 폴더에 넣음
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# cppcheck 시험적으로 추가
set(CMAKE_CXX_CPPCHECK "cppcheck")
find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
if (CMAKE_CXX_CPPCHECK)
    list(
        APPEND CMAKE_CXX_CPPCHECK 
            "--enable=warning"
            "--inconclusive"
            "--force" 
            "--inline-suppr"
            "--library=googletest"
            #"--suppressions-list=${CMAKE_BINARY_DIR}/CppCheckSuppressions.txt"
    )
endif()



# Create build targets for testing
enable_testing()

# 확인할 디렉토리 추가
add_subdirectory(app)
add_subdirectory(lib)
add_subdirectory(tests)

