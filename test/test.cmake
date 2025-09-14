file(GLOB_RECURSE SRC_DIR__TEST CONFIGURE_DEPENDS   "${PRJ_TOP}/test/include/.h"
                                                    "${PRJ_TOP}/test/include/.hh"
                                                    "${PRJ_TOP}/test/*.cc" 
                                                    "${PRJ_TOP}/test/*.c" )


list( APPEND SRC_LIST ${SRC_DIR__TEST})


set( INC_DIR__TEST "")
GET_SUBDIR( INC_DIR__TEST ${PRJ_TOP}/test)

list(APPEND INC_LIST ${PRJ_TOP}/test)
list(APPEND INC_LIST ${INC_DIR__TEST})