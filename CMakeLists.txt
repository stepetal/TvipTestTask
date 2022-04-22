cmake_minimum_required(VERSION 3.0)



project(ffmpegthumbnailer_tester)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/ffmpegthumbnailer/libffmpegthumbnailer)
add_executable(ffmpegthumbnailer_tester main.cpp)

include(ExternalProject)

ExternalProject_Add(project_ffmpegthumbnailer
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ffmpegthumbnailer
  PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/ffmpegthumbnailer
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
)

ExternalProject_Get_Property(project_ffmpegthumbnailer install_dir)
include_directories (${install_dir}/include)

add_library(ffmpegthumbnailer SHARED IMPORTED)
set_property(TARGET ffmpegthumbnailer PROPERTY IMPORTED_LOCATION ${install_dir}/lib/ffmpegthumbnailer.so)
add_dependencies(ffmpegthumbnailer project_ffmpegthumbnailer)


target_link_libraries(ffmpegthumbnailer_tester ${install_dir}/lib/libffmpegthumbnailer.so)



#install(TARGETS ffmpegthumbnailer_tester DESTINATION bin)



