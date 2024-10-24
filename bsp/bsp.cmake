file(GLOB_RECURSE SRC_DIR__BSP CONFIGURE_DEPENDS    "${PRJ_TOP}/bsp/*.h" 
                                                    "${PRJ_TOP}/bsp/*.cc" 
                                                    "${PRJ_TOP}/bsp/*.c" )


list( APPEND SRC_LIST ${SRC_DIR__BSP})


set( INC_DIR__BSP "")
GET_SUBDIR( INC_DIR__BSP ${PRJ_TOP}/bsp)

list(APPEND INC_DIR_LIST ${PRJ_TOP}/bsp)
list(APPEND INC_DIR_LIST ${INC_DIR__BSP})