#########################################################################################################
# NOTE: Native source files need to add manually
#########################################################################################################
set( SRC_LIST_TO_BE_ADDED "")
if( NOT $ENV{METOPE_CHIP} STREQUAL "NATIVE")
    file(GLOB_RECURSE SRC_LIST_TO_BE_ADDED CONFIGURE_DEPENDS    "${PRJ_TOP}/top/*.h" 
                                                                "${PRJ_TOP}/top/*.cc" 
                                                                "${PRJ_TOP}/top/*.c" )
else()
    list( APPEND SRC_LIST_TO_BE_ADDED "${PRJ_TOP}/top/memory.cc")
endif()

message( STATUS "Add the source files ${SRC_LIST_TO_BE_ADDED}")
list( APPEND SRC_LIST ${SRC_LIST_TO_BE_ADDED})


set( INC_LIST_TO_BE_ADDED "")
GET_SUBDIR( INC_LIST_TO_BE_ADDED ${PRJ_TOP}/top)

set( INC_LIST_TO_BE_REMOVED "")


list(APPEND      INC_LIST ${PRJ_TOP}/top)
list(APPEND      INC_LIST ${INC_LIST_TO_BE_ADDED})
list(REMOVE_ITEM INC_LIST ${INC_LIST_TO_BE_REMOVED})