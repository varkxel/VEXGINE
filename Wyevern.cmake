set(Wyevern_CXX_Version cxx_std_23)

set(Wyevern_Version 0.0.0)

set(Wyevern_Platform_Windows INTERNAL "Windows")
set(Wyevern_Platform_UNIX INTERNAL "UNIX")

if(WIN32)
	set(Wyevern_Platform ${Wyevern_Platform_Windows})
	add_compile_definitions(WYEVERN_PLATFORM_WINDOWS)
elseif(UNIX)
	set(Wyevern_Platform ${Wyevern_Platform_UNIX})
	add_compile_definitions(WYEVERN_PLATFORM_UNIX)
endif()

add_compile_definitions(WYEVERN_PLATFORM=${Wyevern_Platform})

function(WyevernExport Project)
	set_target_properties(${Project}
		PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
	)
endfunction()

function(AddWyevernApplication Project Sources)
	add_library(${Project} SHARED ${Sources})
	target_include_directories(${Project} PUBLIC "${WyevernEngine_SOURCE_DIR}")
	WyevernExport(${Project})
endfunction()
