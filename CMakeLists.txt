# CMake 3.12 is required by add_compile_definitions.
cmake_minimum_required(VERSION 3.12)

project(vcmp-anticheat)

# Set the C++ standard to C++11.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Operating-system-specific stuff.
#
# Windows
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
	add_compile_definitions(OS_WINDOWS)
# Linux
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	add_compile_definitions(OS_LINUX)
# Other
else()
	# Unsupported operating system error.
	message(FATAL_ERROR "project must be compiled either on Windows or Linux "
		"operating systems since VC:MP only supports both of them")
endif()

# Compiler-specific stuff (add/remove if needed!).
#
# Microsoft Visual C++
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	add_compile_definitions(COMP_MSVC)
	# Suppress some C4996 warnings.
	add_compile_definitions(_CRT_NONSTDC_NO_WARNINGS _CRT_SECURE_NO_WARNINGS)
# GNU Compiler Collection
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	add_compile_definitions(COMP_GCC)
endif()

# Remove prefix that may be automatically prepended to shared libraries.
set(CMAKE_SHARED_LIBRARY_PREFIX "")

# Define output name suffix for plugins (assuming there are 8 bits in a single
# byte), following VC:MP plugin naming convention (04rel32/64).
math(EXPR CPU_BITS "${CMAKE_SIZEOF_VOID_P} * 8")
set(PLUGIN_OUTPUT_NAME_SUFFIX 04rel${CPU_BITS})

include_directories(src/include)

add_subdirectory(src/anticheat)
