file(GLOB_RECURSE SRC_DIR__TOP CONFIGURE_DEPENDS    "${PRJ_TOP}/top/*.h" 
                                                    "${PRJ_TOP}/top/*.cc" 
                                                    "${PRJ_TOP}/top/*.c" )


list( APPEND SRC_LIST ${SRC_DIR__TOP})


set( INC_DIR__TOP "")
GET_SUBDIR( INC_DIR__TOP ${PRJ_TOP}/top)

list(APPEND INC_DIR_LIST ${PRJ_TOP}/top)
list(APPEND INC_DIR_LIST ${INC_DIR__TOP})