add_custom_command(
    OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/gp4-hdl.pdf
    COMMAND pdflatex -halt-on-error ${CMAKE_CURRENT_SOURCE_DIR}/gp4-hdl.tex
    COMMAND pdflatex -halt-on-error ${CMAKE_CURRENT_SOURCE_DIR}/gp4-hdl.tex
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gp4-hdl.tex
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM)
add_custom_target(doc ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gp4-hdl.pdf)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gp4-hdl.pdf
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc/gp4par/)
