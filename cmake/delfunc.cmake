try_compile (AGGWRAP_DELETED_FUNC_AVAIL "${CMAKE_CURRENT_BINARY_DIR}" 
	SOURCES "${CMAKE_CURRENT_LIST_DIR}/delfunc_test.cpp")
	
if (NOT AGGWRAP_DELETED_FUNC_AVAIL)
	add_definitions ("-DAGGWRAP_NO_DELETED_FUNC")
endif ()
