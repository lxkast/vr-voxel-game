add_subdirectory(glfw)
add_subdirectory(cglm)
add_subdirectory(miniaudio)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_include_directories(${PROJECT_NAME} PRIVATE cglm/include)
target_include_directories(${PROJECT_NAME} PRIVATE glad/include)
target_include_directories(${PROJECT_NAME} PRIVATE miniaudio)