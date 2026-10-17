# Unit Tests CMakeLists.txt

add_executable(test_thread_pool test_thread_pool.cpp)
target_link_libraries(test_thread_pool PRIVATE pfs_core)
add_test(NAME ThreadPoolTest COMMAND test_thread_pool)

add_executable(test_search_engine test_search_engine.cpp)
target_link_libraries(test_search_engine PRIVATE pfs_core)
add_test(NAME SearchEngineTest COMMAND test_search_engine)

add_executable(test_cancellation test_cancellation.cpp)
target_link_libraries(test_cancellation PRIVATE pfs_core)
add_test(NAME CancellationTest COMMAND test_cancellation)

add_executable(test_export test_export.cpp)
target_link_libraries(test_export PRIVATE pfs_core)
add_test(NAME ExportTest COMMAND test_export)
