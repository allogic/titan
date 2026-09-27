ADD_LIBRARY (glslang INTERFACE)

TARGET_COMPILE_DEFINITIONS (glslang INTERFACE "_CRT_SECURE_NO_WARNINGS")

TARGET_INCLUDE_DIRECTORIES (glslang INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
	TARGET_LINK_LIBRARIES (glslang INTERFACE
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/SPIRVd.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/SPIRV-Toolsd.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/SPIRV-Tools-optd.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/GenericCodeGend.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/glslangd.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/glslang-default-resource-limitsd.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/MachineIndependentd.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/OSDependentd.lib"
)
ELSEIF (CMAKE_BUILD_TYPE STREQUAL "Release")
	TARGET_LINK_LIBRARIES (glslang INTERFACE
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/release/SPIRV.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/release/SPIRV-Tools.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/release/SPIRV-Tools-opt.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/release/GenericCodeGen.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/release/glslang.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/release/glslang-default-resource-limits.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/release/MachineIndependent.lib"
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/release/OSDependent.lib"
)
ELSE ()
	MESSAGE (FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")
ENDIF ()