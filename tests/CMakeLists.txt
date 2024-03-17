include(GoogleTest)


# 현재 디렉토리에 있는 모든 파일을 SRC_FILES 변수에 추가한다
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cc)
set(SOURCES ${TEST_SOURCES})

find_package(GTest REQUIRED)

add_executable(gtest_all ${SRC_FILES})
target_link_libraries(gtest_all PRIVATE  common  gtest gmock gtest_main  ${CMAKE_THREAD_LIBS_INIT})



gtest_discover_tests(gtest_all TEST_PREFIX)



