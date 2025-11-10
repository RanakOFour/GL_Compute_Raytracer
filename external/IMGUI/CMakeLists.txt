file(GLOB SRCS *.cpp)

target_sources(GCP_Raytracer_Framework_CMAKE PUBLIC ${SRCS})

target_include_directories(GCP_Raytracer_Framework_CMAKE PUBLIC external/IMGUI)