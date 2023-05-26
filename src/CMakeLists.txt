get_filename_component(APP iengine NAME)
add_executable(${APP} main.cc)
add_library(ie clause.cc iengine.cc fc_iengine.cc bc_iengine.cc tt_iengine.cc)
target_link_libraries(${APP} ie)
