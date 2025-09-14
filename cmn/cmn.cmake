
#########################################################################################################
# NOTE: Native source files need to add manually
#########################################################################################################
if( $ENV{METOPE_CHIP} STREQUAL "NATIVE")
    list( APPEND SRC_DIR__CMN   "${PRJ_TOP}/cmn/cmn_utility.c"
                                "${PRJ_TOP}/cmn/cmn_math.c")
else()
    file(GLOB_RECURSE SRC_DIR__CMN CONFIGURE_DEPENDS    "${PRJ_TOP}/cmn/*.h" 
                                                        "${PRJ_TOP}/cmn/*.cc" 
                                                        "${PRJ_TOP}/cmn/*.c" )
endif()


list( APPEND SRC_LIST ${SRC_DIR__CMN})


set( INC_DIR__CMN "")
GET_SUBDIR( INC_DIR__CMN ${PRJ_TOP}/cmn)

list(APPEND INC_LIST ${PRJ_TOP}/cmn)
list(APPEND INC_LIST ${INC_DIR__CMN})