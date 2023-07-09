add_executable(GMTK2023
  main.cpp
  board.cpp)
target_link_libraries(GMTK2023 GraphicsEnv)

if(UNIX) # want linux binary to check current dir for libraries
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-rpath='${ORIGIN}'")
    if(GRAPHICS_BUILD_STATIC)
      target_link_libraries(GMTK2023 -static-libgcc -static-libstdc++)
    endif()
  endif()
  target_link_libraries(GMTK2023 -pthread)
endif()

add_custom_command(TARGET GMTK2023 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/../resources"
        $<TARGET_FILE_DIR:GMTK2023>)

if(NOT NO_ASSIMP)
  if(NOT BUILD_ASSIMP_STATIC)	
    add_custom_command(TARGET GMTK2023 POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
      $<TARGET_FILE_DIR:assimp>
      $<TARGET_FILE_DIR:GMTK2023>)
  endif()
endif()

if(NOT NO_FREETYPE)
  if(NOT VKENV_BUILD_STATIC)
    add_custom_command(TARGET GMTK2023 POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
      $<TARGET_FILE_DIR:freetype>
      $<TARGET_FILE_DIR:GMTK2023>)
  endif()
endif()

if(NOT VKENV_BUILD_STATIC)
  add_custom_command(TARGET GMTK2023 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    $<TARGET_FILE_DIR:glfw>
    $<TARGET_FILE_DIR:GMTK2023>)
endif()
