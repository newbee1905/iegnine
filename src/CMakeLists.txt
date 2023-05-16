get_filename_component(APP iengine NAME)
add_library(ie inference_engine.cc statement.cc)
add_executable(${APP} main.cc)
target_link_libraries(${APP} ie)
