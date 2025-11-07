cmake_minimum_required (VERSION 3.10)

project ("GCP_Raytracer_Framework_CMAKE")

# Add source to this project's executable.
add_executable (GCP_Raytracer_Framework_CMAKE "main.cpp")

add_subdirectory(src)
add_subdirectory(external)

target_include_directories(GCP_Raytracer_Framework_CMAKE PUBLIC external/IMGUI include)
target_link_directories(GCP_Raytracer_Framework_CMAKE PUBLIC ${PROJECT_SOURCE_DIR}/lib)

if(WIN32)
	target_link_libraries(GCP_Raytracer_Framework_CMAKE PUBLIC SDL2 SDL2main opengl32)
else()
	target_link_libraries(GCP_Raytracer_Framework_CMAKE PUBLIC SDL2 SDL2main GL)
endif()

# TODO: Add tests and install targets if needed.
#add_custom_command(TARGET GCP_Raytracer_Framework_CMAKE POST_BUILD {CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/resources" $<TARGET_FILE_DIR:GCP_Raytracer_Framework_CMAKE>/resources/)
add_custom_command(TARGET GCP_Raytracer_Framework_CMAKE POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
				   "${PROJECT_SOURCE_DIR}/resources"
				   $<TARGET_FILE_DIR:GCP_Raytracer_Framework_CMAKE>/resources/)