# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (Tutorials)

find_package(OpenGL REQUIRED)


if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

# Initialize CXXFLAGS.
#set(CMAKE_CXX_FLAGS                "-Wall -std=c++11")
#set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
#set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
#set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
#set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

# Compiler-specific C++11 activation.
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
        message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater.")
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	set(CMAKE_CXX_FLAGS                "-Wall -std=c++11")
	set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
	set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
endif ()

# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-1c73e35")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)


include_directories(
	external/AntTweakBar-1.16/include/
	external/glfw-3.0.3/include/GLFW/
	external/glm-0.9.4.0/
	external/glew-1.9.0/include/
	external/stb_image/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	GLFW_303
	GLEW_190
	ANTTWEAKBAR_116_OGLCORE_GLFW
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

# Tutorial 01
add_executable(tutorial01_basic_shader 
	common/shader.hpp
	common/shader.cpp
	common/texture.hpp
	common/texture.cpp
	common/controls.hpp
	common/controls.cpp
	common/objloader.hpp
	common/objloader.cpp
	common/vboindexer.hpp
	common/vboindexer.cpp
	common/text2D.hpp
	common/text2D.cpp
	common/model.hpp
	common/model.cpp
	common/tangentspace.hpp
	common/tangentspace.cpp
	common/quaternion_utils.hpp
	common/quaternion_utils.cpp
	Tutorial_1_Basic_Shader/Tutorial_01_BasicShader.cpp
)
target_link_libraries(tutorial01_basic_shader
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(tutorial01_basic_shader PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tutorial_1/")
create_target_launcher(tutorial01_basic_shader WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tutorial_1/")

# Tutorial 02
add_executable(tutorial02_transform_texture 
	common/shader.hpp
	common/shader.cpp
	common/texture.hpp
	common/texture.cpp
	common/controls.hpp
	common/controls.cpp
	common/objloader.hpp
	common/objloader.cpp
	common/vboindexer.hpp
	common/vboindexer.cpp
	common/text2D.hpp
	common/text2D.cpp
	common/model.hpp
	common/model.cpp
	common/tangentspace.hpp
	common/tangentspace.cpp
	common/quaternion_utils.hpp
	common/quaternion_utils.cpp
	Tutorial_2_Transform_Texture/Tutorial_02_Transform_Texture.cpp
)
target_link_libraries(tutorial02_transform_texture
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(tutorial02_transform_texture PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Tutorial_2_Transform_Texture/")
create_target_launcher(tutorial02_transform_texture WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Tutorial_2_Transform_Texture/")

# Tutorial 03
add_executable(tutorial03_basic_lighting 
	common/shader.hpp
	common/shader.cpp
	common/texture.hpp
	common/texture.cpp
	common/controls.hpp
	common/controls.cpp
	common/objloader.hpp
	common/objloader.cpp
	common/vboindexer.hpp
	common/vboindexer.cpp
	common/text2D.hpp
	common/text2D.cpp
	common/model.hpp
	common/model.cpp
	common/tangentspace.hpp
	common/tangentspace.cpp
	common/quaternion_utils.hpp
	common/quaternion_utils.cpp
	Tutorial_3_Basic_Lighting/Tutorial_03_Basic_Lighting.cpp
)
target_link_libraries(tutorial03_basic_lighting
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(tutorial03_basic_lighting PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Tutorial_3_Basic_Lighting/")
create_target_launcher(tutorial03_basic_lighting WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Tutorial_3_Basic_Lighting/")



#tutorial 04
add_executable(tutorial04_GeometryShader 
	common/shader.hpp
	common/shader.cpp
	common/texture.hpp
	common/texture.cpp
	common/controls.hpp
	common/controls.cpp
	common/objloader.hpp
	common/objloader.cpp
	common/vboindexer.hpp
	common/vboindexer.cpp
	common/text2D.hpp
	common/text2D.cpp
	common/model.hpp
	common/model.cpp
	common/tangentspace.hpp
	common/tangentspace.cpp
	common/quaternion_utils.hpp
	common/quaternion_utils.cpp
	Tutorial_4_GeometryShader/Tutorial_04_GeometryShader.cpp
)

target_link_libraries(tutorial04_GeometryShader
	${ALL_LIBS}
)

# Xcode and Visual working directories
set_target_properties(tutorial04_GeometryShader PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Tutorial_4_GeometryShader/")
create_target_launcher(tutorial04_GeometryShader WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Tutorial_4_GeometryShader/")


#tutorial 05
add_executable(tutorial05_TessellationShader 
	common/shader.hpp
	common/shader.cpp
	common/texture.hpp
	common/texture.cpp
	common/controls.hpp
	common/controls.cpp
	common/objloader.hpp
	common/objloader.cpp
	common/vboindexer.hpp
	common/vboindexer.cpp
	common/text2D.hpp
	common/text2D.cpp
	common/model.hpp
	common/model.cpp
	common/tangentspace.hpp
	common/tangentspace.cpp
	common/quaternion_utils.hpp
	common/quaternion_utils.cpp
	Tutorial_5_TessellationShader/Tutorial_05_TessellationShader.cpp
)

target_link_libraries(tutorial05_TessellationShader
	${ALL_LIBS}
)

# Xcode and Visual working directories
set_target_properties(tutorial05_TessellationShader PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Tutorial_5_TessellationShader/")
create_target_launcher(tutorial05_TessellationShader WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Tutorial_5_TessellationShader/")

#tutorial 06
add_executable(tutorial06_PhysicallyBasedShading 
	common/shader.hpp
	common/shader.cpp
	common/texture.hpp
	common/texture.cpp
	common/controls.hpp
	common/controls.cpp
	common/objloader.hpp
	common/objloader.cpp
	common/vboindexer.hpp
	common/vboindexer.cpp
	common/text2D.hpp
	common/text2D.cpp
	common/model.hpp
	common/model.cpp
	common/tangentspace.hpp
	common/tangentspace.cpp
	common/quaternion_utils.hpp
	common/quaternion_utils.cpp
	Tutorial_6_PhysicallyBasedShading/Tutorial_6_PhysicallyBasedShading.cpp
)

target_link_libraries(tutorial06_PhysicallyBasedShading
	${ALL_LIBS}	
)

# Xcode and Visual working directories
set_target_properties(tutorial06_PhysicallyBasedShading PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Tutorial_6_PhysicallyBasedShading/")
create_target_launcher(tutorial06_PhysicallyBasedShading WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Tutorial_6_PhysicallyBasedShading/")


#Tutorial 07
aux_source_directory("Tutorial_7_SimpleGeometryClipmap" tutorial07_src)
message(${tutorial07_src})
add_executable(tutorial07_SimpleGeometryClipmap 
	common/shader.hpp
	common/shader.cpp
	common/texture.hpp
	common/texture.cpp
	common/controls.hpp
	common/controls.cpp
	common/objloader.hpp
	common/objloader.cpp
	common/vboindexer.hpp
	common/vboindexer.cpp
	common/text2D.hpp
	common/text2D.cpp
	common/model.hpp
	common/model.cpp
	common/tangentspace.hpp
	common/tangentspace.cpp
	common/quaternion_utils.hpp
	common/quaternion_utils.cpp
	${tutorial07_src}
)
target_link_libraries(tutorial07_SimpleGeometryClipmap
	${ALL_LIBS}
)

# Xcode and Visual working directories
set_target_properties(tutorial07_SimpleGeometryClipmap PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Tutorial_7_SimpleGeometryClipmap/")
create_target_launcher(tutorial07_SimpleGeometryClipmap WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Tutorial_7_SimpleGeometryClipmap/")


# Tutorial 02
if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
add_custom_command(
	   TARGET tutorial01_basic_shader POST_BUILD
	   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/models ${CMAKE_BINARY_DIR}/models
)

add_custom_command(
	   TARGET tutorial01_basic_shader POST_BUILD
	   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Tutorial_1_Basic_Shader/shaders ${CMAKE_BINARY_DIR}/shaders
)
	   
add_custom_command(
	   	   TARGET tutorial02_transform_texture POST_BUILD
	   	   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Tutorial_2_Transform_Texture/shaders ${CMAKE_BINARY_DIR}/shaders
)

add_custom_command(
	   	   TARGET tutorial03_basic_lighting POST_BUILD
	   	   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Tutorial_3_Basic_Lighting/shaders ${CMAKE_BINARY_DIR}/shaders
)

add_custom_command(
	   	   TARGET tutorial04_GeometryShader POST_BUILD
	   	   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Tutorial_4_GeometryShader/shaders ${CMAKE_BINARY_DIR}/shaders
)

add_custom_command(
	   	   TARGET tutorial05_TessellationShader POST_BUILD
	   	   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Tutorial_5_TessellationShader/shaders ${CMAKE_BINARY_DIR}/shaders
)

add_custom_command(
	   	   TARGET tutorial06_PhysicallyBasedShading POST_BUILD
	   	   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Tutorial_6_PhysicallyBasedShading/shaders ${CMAKE_BINARY_DIR}/shaders
)

add_custom_command(
	   	   TARGET tutorial07_SimpleGeometryClipmap POST_BUILD
	   	   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Tutorial_7_SimpleGeometryClipmap/shaders ${CMAKE_BINARY_DIR}/shaders
		   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Tutorial_7_SimpleGeometryClipmap/textures ${CMAKE_BINARY_DIR}/textures
		   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Tutorial_7_SimpleGeometryClipmap/models ${CMAKE_BINARY_DIR}/models
)


elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

