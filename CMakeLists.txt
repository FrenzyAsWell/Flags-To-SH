cmake_minimum_required (VERSION 3.11)

project(ftxui-starter LANGUAGES CXX VERSION 1.0.0)
find_package(ftxui 5 REQUIRED)

add_executable(ftxui-starter src/gui.cpp)
target_link_libraries(ftxui-starter
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component # Not needed for this example.
)
