get_filename_component(APP iengine NAME)
add_executable(${APP} main.cc)
add_library(ie inference_engine.cc FCIEngine.cc)
target_link_libraries(${APP} ie)
