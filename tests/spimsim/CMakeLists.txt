
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/spim)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/CPU)
# 현재 디렉토리에 있는 모든 파일을 SRC_FILES 변수에 추가한다
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp,
    ${CMAKE_CURRENT_SOURCE_DIR}/spim/*.cpp,
    ${CMAKE_CURRENT_SOURCE_DIR}/spim/*.h,
    ${CMAKE_CURRENT_SOURCE_DIR}/CPU/*.h,
    ${CMAKE_CURRENT_SOURCE_DIR}/CPU/*.cpp,
    ${CMAKE_CURRENT_SOURCE_DIR}/CPU/*.c,

)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false ./spim/*.h ./spim/*.cc)


add_executable(spim ${SRC_FILES})
target_link_libraries(spim PRIVATE spim string-stream spim-utils inst reg men parser sym-tbl scanner parser_yacc data ${CMAKE_THREAD_LIBS_INIT})

#include "spim.h"
#include "string-stream.h"
#include "spim-utils.h"
#include "inst.h"
#include "reg.h"
#include "mem.h"
#include "parser.h"
#include "sym-tbl.h"
#include "scanner.h"
#include "parser_yacc.h"
#include "data.h"


gtest_discover_tests(spim TEST_PREFIX)



