file(GLOB SRCS *.cpp)

target_sources(GLTracer PUBLIC ${SRCS})

target_include_directories(GLTracer SYSTEM PUBLIC external/IMGUI)
