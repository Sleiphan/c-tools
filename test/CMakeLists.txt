
set(TEST "stack")
add_executable(${TEST} stack.cpp)
target_link_libraries(${TEST} PRIVATE ${CTOOLS_LIB} GTest::gtest_main)
gtest_discover_tests(${TEST})

set(TEST "btree")
add_executable(${TEST} btree.cpp)
target_link_libraries(${TEST} PRIVATE ${CTOOLS_LIB} GTest::gtest_main)
gtest_discover_tests(${TEST})

set(TEST "huffman")
add_executable(${TEST} huffman.cpp)
target_link_libraries(${TEST} PRIVATE ${CTOOLS_LIB} GTest::gtest_main)
gtest_discover_tests(${TEST})
