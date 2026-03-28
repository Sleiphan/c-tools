add_library(${CTOOLS_LIB} SHARED)
target_include_directories(${CTOOLS_LIB} PUBLIC ${CMAKE_SOURCE_DIR}/include)
set_target_properties(${CTOOLS_LIB} PROPERTIES 
    LINKER_LANGUAGE C
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

add_subdirectory(${CTOOLS_LIB})

configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/${CTOOLS_LIB}.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/${CTOOLS_LIB}.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${CTOOLS_LIB}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    COMPONENT dev
)

# Install shared library (runtime package)
install(TARGETS ${CTOOLS_LIB}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT runtime
)

# Install headers (dev package)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT dev
)