file(GLOB_RECURSE SRC_DIR__APP CONFIGURE_DEPENDS    "${PRJ_TOP}/app/*.h" 
                                                    "${PRJ_TOP}/app/*.cc" 
                                                    "${PRJ_TOP}/app/*.c" )


list( APPEND SRC_LIST ${SRC_DIR__APP})



if($ENV{METOPE_CHIP} STREQUAL "NATIVE")
    list( APPEND SRC_LIST_TO_BE_REMOVED     "${PRJ_TOP}/app/app_lvgl.c"
                                            "${PRJ_TOP}/app/app_rtos_heap_4.c"
                                            "${PRJ_TOP}/app/app_rtos.c"
                                            "${PRJ_TOP}/app/app_clock.c" )

    message( STATUS "Remove the source files: ${SRC_LIST_TO_BE_REMOVED}")
    list( REMOVE_ITEM SRC_LIST ${SRC_LIST_TO_BE_REMOVED})
endif()

set( INC_DIR__APP "")
GET_SUBDIR( INC_DIR__APP ${PRJ_TOP}/app)

list(APPEND INC_LIST ${PRJ_TOP}/app)
list(APPEND INC_LIST ${INC_DIR__APP})