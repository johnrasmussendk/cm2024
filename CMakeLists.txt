cmake_minimum_required(VERSION 3.0)

project(cm2024)

# Create a list with all .cpp source files
set(project_sources
   main.cpp
   DatMessage.cpp
)

# Create executable with all necessary source files
add_executable(${PROJECT_NAME} ${project_sources})

install(TARGETS cm2024 RUNTIME DESTINATION bin)
