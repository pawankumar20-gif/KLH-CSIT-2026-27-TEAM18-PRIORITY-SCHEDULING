# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  [[CMakeFiles\ParallelFileSearch_autogen.dir\AutogenUsed.txt]]
  [[CMakeFiles\ParallelFileSearch_autogen.dir\ParseCache.txt]]
  [[CMakeFiles\pfs_core_autogen.dir\AutogenUsed.txt]]
  [[CMakeFiles\pfs_core_autogen.dir\ParseCache.txt]]
  "ParallelFileSearch_autogen"
  "pfs_core_autogen"
  [[tests\CMakeFiles\test_cancellation_autogen.dir\AutogenUsed.txt]]
  [[tests\CMakeFiles\test_cancellation_autogen.dir\ParseCache.txt]]
  [[tests\CMakeFiles\test_export_autogen.dir\AutogenUsed.txt]]
  [[tests\CMakeFiles\test_export_autogen.dir\ParseCache.txt]]
  [[tests\CMakeFiles\test_search_engine_autogen.dir\AutogenUsed.txt]]
  [[tests\CMakeFiles\test_search_engine_autogen.dir\ParseCache.txt]]
  [[tests\CMakeFiles\test_thread_pool_autogen.dir\AutogenUsed.txt]]
  [[tests\CMakeFiles\test_thread_pool_autogen.dir\ParseCache.txt]]
  [[tests\test_cancellation_autogen]]
  [[tests\test_export_autogen]]
  [[tests\test_search_engine_autogen]]
  [[tests\test_thread_pool_autogen]]
  )
endif()
