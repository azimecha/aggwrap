cmake_minimum_required (VERSION 3.0)

include (cmake/inline.cmake)

option (WRAPPER_SHARED "Build aggwrap as shared library" ON)
option (WRAPPER_STATIC_CRT "Static link C runtime to aggwrap" OFF)

if (WRAPPER_STATIC_CRT)
	if (MSVC)
		add_compile_options ($<$<CONFIG:Debug>:/MTd> $<$<NOT:$<CONFIG:Debug>>:/MT>)
	endif ()
endif ()

add_subdirectory (agg)

file (GLOB WRAPPER_CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file (GLOB WRAPPER_C_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
file (GLOB_RECURSE WRAPPER_HPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)

if (WRAPPER_SHARED)
	add_library (aggwrap SHARED ${WRAPPER_CPP_FILES} ${WRAPPER_C_FILES} ${WRAPPER_HPP_FILES} inc/AGGWrap.h)
	target_compile_definitions (aggwrap PUBLIC AGGWRAP_SHARED)
else ()
	add_library (aggwrap STATIC ${WRAPPER_CPP_FILES} ${WRAPPER_C_FILES} ${WRAPPER_HPP_FILES} inc/AGGWrap.h)
endif ()

target_compile_definitions (aggwrap PRIVATE AGGWRAP_BUILD)
target_link_libraries (aggwrap AGG)

target_include_directories (aggwrap PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_include_directories (aggwrap PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/stb
)
