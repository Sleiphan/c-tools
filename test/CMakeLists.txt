
set(TEST "T-stack")
add_executable(${TEST} stack.cpp)
target_link_libraries(${TEST} PRIVATE ${CTOOLS_LIB} GTest::gtest_main)
gtest_discover_tests(${TEST})

set(TEST "T-stack_concurrency")
add_executable(${TEST} stack_concurrency.cpp)
target_link_libraries(${TEST} PRIVATE ${CTOOLS_LIB} GTest::gtest_main)
gtest_discover_tests(${TEST})

set(TEST "T-btree")
add_executable(${TEST} btree.cpp)
target_link_libraries(${TEST} PRIVATE ${CTOOLS_LIB} GTest::gtest_main)
gtest_discover_tests(${TEST})

set(TEST "T-huffman")
add_executable(${TEST} huffman.cpp)
target_link_libraries(${TEST} PRIVATE ${CTOOLS_LIB} GTest::gtest_main)
gtest_discover_tests(${TEST})

set(TEST "T-queue")
add_executable(${TEST} queue.cpp)
target_link_libraries(${TEST} PRIVATE ${CTOOLS_LIB} GTest::gtest_main)
gtest_discover_tests(${TEST})

set(TEST "T-heap")
add_executable(${TEST} heap.cpp)
target_link_libraries(${TEST} PRIVATE ${CTOOLS_LIB} GTest::gtest_main)
gtest_discover_tests(${TEST})
