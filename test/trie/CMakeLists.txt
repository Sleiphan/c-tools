set(TEST "T-trie")
add_executable(${TEST} trie.cpp)
target_link_libraries(${TEST} PRIVATE ${CTOOLS_LIB} GTest::gtest_main)
gtest_discover_tests(${TEST})

set(TEST "T-rtree")
add_executable(${TEST} rtree.cpp)
target_link_libraries(${TEST} PRIVATE ${CTOOLS_LIB} GTest::gtest_main)
gtest_discover_tests(${TEST})