try_compile (AGGWRAP_CONSTEXPR_STD "${CMAKE_CURRENT_BINARY_DIR}" 
	SOURCES "${CMAKE_CURRENT_LIST_DIR}/constexpr_test.cpp")
	
if (NOT AGGWRAP_CONSTEXPR_STD)
	add_definitions ("-Dconstexpr=const")
endif ()
