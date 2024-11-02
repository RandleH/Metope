file(GLOB_RECURSE SRC_DIR__APP CONFIGURE_DEPENDS    "${PRJ_TOP}/app/*.h" 
                                                    "${PRJ_TOP}/app/*.cc" 
                                                    "${PRJ_TOP}/app/*.c" )


list( APPEND SRC_LIST ${SRC_DIR__APP})


set( INC_DIR__APP "")
GET_SUBDIR( INC_DIR__APP ${PRJ_TOP}/app)

list(APPEND INC_DIR_LIST ${PRJ_TOP}/app)
list(APPEND INC_DIR_LIST ${INC_DIR__APP})