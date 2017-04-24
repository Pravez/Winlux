enable_testing()

set(PROJECT_SOURCE ../src/context.c ../src/context.h ../src/queue/queue.h ../src/queue/o_queue.h
        ../src/queue/o_queue.c ../src/thread.c ../src/queue/o_list.c ../src/queue/o_list.h)

add_executable(main 01-main.c ${PROJECT_SOURCE})
add_executable(switch 02-switch.c ${PROJECT_SOURCES})
add_executable(join 11-join.c ${PROJECT_SOURCES})
add_executable(joinmain 12-join-main.c ${PROJECT_SOURCES})
add_executable(createmany 21-create-many.c ${PROJECT_SOURCES})
add_executable(createmanyrecursive 22-create-many-recursive.c ${PROJECT_SOURCES})
add_executable(createmanyonce 23-create-many-once.c ${PROJECT_SOURCES})
add_executable(switchmany 31-switch-many.c ${PROJECT_SOURCES})
add_executable(switchmanyjoin 32-switch-many-join.c ${PROJECT_SOURCES})
add_executable(fibonacci 51-fibonacci.c ${PROJECT_SOURCES})
add_executable(mutex 61-mutex.c ${PROJECT_SOURCES})

add_test(01-main main)
add_test(02-switch switch)
add_test(11-join join)
add_test(12-join_main joinmain)
add_test(21-create_many createmany)
add_test(22-create_many_recursive createmanyrecursive)
add_test(23-create_many_once createmanyonce)
add_test(31-switch_many switchmany)
add_test(32-switch_many_join switchmanyjoin)
add_test(51-fibonacci fibonacci)

target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})
