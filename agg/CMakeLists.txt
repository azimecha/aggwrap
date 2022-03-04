file (GLOB AGG_CPP_FILES *.cpp)
file (GLOB AGG_H_FILES *.h)

add_library (AGG STATIC ${AGG_CPP_FILES} ${AGG_H_FILES})
target_compile_definitions (AGG PUBLIC AGG_STATIC_DEFINE)

target_include_directories (AGG
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
	PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/exports)

include (GenerateExportHeader)
generate_export_header (AGG EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/exports/agg_export.h)
