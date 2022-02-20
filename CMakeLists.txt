cmake_minimum_required(VERSION 3.0)

project(cm2024)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to create code from Qt designer ui files
set(CMAKE_AUTOUIC ON)


# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

# Create a list with all .cpp source files
set(project_sources
   main.cpp
   datmessage.cpp
   mainwindow.cpp
   usbworker.cpp
)

# Create executable with all necessary source files
add_executable(${PROJECT_NAME} ${project_sources})

# Use the Widgets module from Qt 5.
target_link_libraries(cm2024 Qt5::Widgets)

install(TARGETS cm2024 RUNTIME DESTINATION bin)
