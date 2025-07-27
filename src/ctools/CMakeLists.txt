target_sources(${CTOOLS_LIB} PUBLIC 
    btree.c
    stack.c
    queue.c
    heap.c
)

add_subdirectory(btree_ext)