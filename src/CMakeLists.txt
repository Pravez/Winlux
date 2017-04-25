add_library(TThreads context.c context.h queue/queue.h queue/o_queue.h queue/o_queue.c thread.c queue/o_list.c queue/o_list.h)
add_executable(Project main.c)
target_link_libraries(Project ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(Project TThreads)

