target_sources(${CTOOLS_LIB} PUBLIC 
    btree.c
    queue.c
    cbuf.c
)

add_subdirectory(btree_ext)