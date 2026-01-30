cmake_minimum_required (VERSION 3.20)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CXX_STANDARD 20)

project ("GLTracer")

add_executable (GLTracer "main.cpp")

add_subdirectory(src)
add_subdirectory(external)


target_include_directories(GLTracer PUBLIC include)

target_include_directories(GLTracer SYSTEM PUBLIC external/IMGUI external)

target_link_directories(GLTracer PUBLIC ${PROJECT_SOURCE_DIR}/lib)

if(WIN32)
	target_link_libraries(GLTracer PUBLIC SDL2 SDL2main opengl32)
else()
	target_link_libraries(GLTracer PUBLIC SDL2 SDL2main GL)
endif()

# TODO: Add tests and install targets if needed.
#add_custom_command(TARGET GLTracer POST_BUILD {CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/resources" $<TARGET_FILE_DIR:GLTracer>/resources/)
add_custom_command(TARGET GLTracer POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
				   "${PROJECT_SOURCE_DIR}/resources"
				   $<TARGET_FILE_DIR:GLTracer>/resources/)

add_custom_command(TARGET GLTracer POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
				   "${PROJECT_SOURCE_DIR}/lib"
				   "$<TARGET_FILE_DIR:GLTracer>/")