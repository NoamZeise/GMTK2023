cmake_minimum_required(VERSION 3.14)

set(ProjectName "GMTK2023")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

project(Graphics-Env-Proj VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
  
add_subdirectory(libs)
add_subdirectory(src)
