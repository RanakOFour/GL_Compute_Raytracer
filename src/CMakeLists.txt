file(GLOB SRCS *.cpp
			   glew.c)

target_sources(GLTracer PUBLIC ${SRCS})