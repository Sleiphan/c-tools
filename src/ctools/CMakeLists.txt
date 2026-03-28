target_sources(${CTOOLS_LIB} PUBLIC 
    btree.c
    cbuf.c
)

add_subdirectory(btree_ext)
add_subdirectory(trie)
