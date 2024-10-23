file(GLOB_RECURSE SRC_DIR__CMN CONFIGURE_DEPENDS    "${PRJ_TOP}/cmn/*.h" 
                                                    "${PRJ_TOP}/cmn/*.cc" 
                                                    "${PRJ_TOP}/cmn/*.c" )


list( APPEND SRC_LIST ${SRC_DIR__CMN})


set( INC_DIR__CMN "")
GET_SUBDIR( INC_DIR__CMN ${PRJ_TOP}/cmn)

list(APPEND INC_DIR_LIST ${PRJ_TOP}/cmn)
list(APPEND INC_DIR_LIST ${INC_DIR__CMN})