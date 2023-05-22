get_filename_component(APP iengine NAME)
add_executable(${APP} main.cc)
add_library(ie iengine.cc FCIEngine.cc BCIEngine.cc)
target_link_libraries(${APP} ie)
