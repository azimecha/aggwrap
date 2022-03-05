try_compile (AGGWRAP_INLINE_STD "${CMAKE_CURRENT_BINARY_DIR}" 
	SOURCES "${CMAKE_CURRENT_LIST_DIR}/inline_test.c")
	
if (NOT AGGWRAP_INLINE_STD)
	try_compile (AGGWRAP_INLINE_NONSTD_A "${CMAKE_CURRENT_BINARY_DIR}" 
		SOURCES "${CMAKE_CURRENT_LIST_DIR}/inline_test.c" 
		COMPILE_DEFINITIONS "-Dinline=__inline__")
		
	if (AGGWRAP_INLINE_NONSTD_A)
		add_definitions (-Dinline=__inline__)
	else ()
		try_compile (AGGWRAP_INLINE_NONSTD_B "${CMAKE_CURRENT_BINARY_DIR}" 
			SOURCES "${CMAKE_CURRENT_LIST_DIR}/inline_test.c" 
			COMPILE_DEFINITIONS "-Dinline=__inline")
			
		if (AGGWRAP_INLINE_NONSTD_B)
			add_definitions (-Dinline=__inline)
		else ()
			add_definitions ("-Dinline=")
		endif ()
	endif ()
endif ()
