set(SOURCE_FILES example.c thread.h)

add_executable(Example ${SOURCE_FILES})
target_link_libraries(Example ${CMAKE_THREAD_LIBS_INIT})