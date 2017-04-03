set(SOURCE_FILES example.c thread.h)

add_executable(Example ${SOURCE_FILES})
target_link_libraries(Example ${CMAKE_THREAD_LIBS_INIT})


set(EXAMPLE_SOURCES contextes.c)
add_executable(Contextes ${EXAMPLE_SOURCES})
target_link_libraries(Example ${CMAKE_THREAD_LIBS_INIT})

set(PROJECT_SOURCE context.c context.h main.c)
add_executable(Project ${PROJECT_SOURCE})
